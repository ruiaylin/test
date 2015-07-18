/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2002 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "udpsession.h"
#include "myicq.h"
#include "ndes.h"
#include "log.h"
#include "utils.h"
#include "udppacket.h"
#include "sessionhash.h"
#include "server.h"
#include "groupplugin.h"


#define MYICQ_UDP_VER		1
#define MAX_SEND_ATTEMPTS	2
#define MAX_SQL				4096
#define MAX_MSG_LEN			450
#define ADMIN_UIN			10000

#define MAX_ONLINES_PER_PACKET	48
#define MAX_SEARCH				25
#define LIMIT_SEARCH			" LIMIT 25"
#define LIMIT_FRIENDS			" LIMIT 256"

#define MAX_BCMSG			8
#define BCMSG_INDEX(i)		(i & (MAX_BCMSG - 1))


enum {
	LOGIN_SUCCESS,
	LOGIN_INVALID_UIN,
	LOGIN_WRONG_PASSWD,
};

enum {
	ADD_FRIEND_ACCEPTED,
	ADD_FRIEND_AUTH_REQ,
	ADD_FRIEND_REJECTED,
};

enum {
	GROUP_ERROR_SUCCESS,
	GROUP_ERROR_NOT_EXIST,
	GROUP_ERROR_ALREADY_EXIST,
	GROUP_ERROR_EXCEED_MAX_GROUPS,
	GROUP_ERROR_WRONG_PASSWD,
	GROUP_ERROR_EXCEED_MAX_MEMBERS,
};

class BCMsg {
public:
	BCMsg() {
		id = 0;
		text.text = buf;
	}

	uint32 id;
	uint8 type;
	uint32 when;
	ICQ_STR text;
	char buf[MAX_MSG_LEN + 1];
};

static ListHead		globalSendQueue;
static ListHead		keepAliveList;
static BCMsg		bcMsgQueue[MAX_BCMSG];
static int			bcMsgPos;

static ICQ_STR emptyStr = { "", 0 };
static QID qidAdmin;

int UdpSession::sock = -1;
uint32 UdpSession::sessionCount;

// Slab
IMPLEMENT_SLAB(UdpSession, 128)


static void dbAddFriend(QID &dst, QID &src)
{
	Session *s;
	DBRequest *req = new DBRequest(DBF_UPDATE);

	if (src.domain.len) {
		WRITE_STR(req, "INSERT INTO notify_tbl VALUES(");
		*req << dst.uin << ',' << src.uin << ',' << src.domain;

		s = SessionHash::get(dst.uin);
		if (s && s->status != STATUS_INVIS) {
			Server *server = Server::createServer(src.domain.text);
			server->sendUserOnline(s);
		}

	} else {
		if (dst.domain.len) {
			s = Server::getSession(dst);

			WRITE_STR(req, "INSERT INTO remote_friend_tbl VALUES(");
			*req << src.uin << ',' << dst.uin << ',' << dst.domain;
		} else {
			s = SessionHash::get(dst.uin);

			WRITE_STR(req, "INSERT INTO friend_tbl VALUES(");
			*req << src.uin << ',' << dst.uin;
		}

		if (s && s->status != STATUS_INVIS) {
			UdpSession *session = SessionHash::get(src.uin);
			if (session)
				session->sendOnline(s, dst.domain);
		}
	}

	*req << ')';
	DBManager::query(req);
}

static int addBCMsg(uint32 id, uint8 type, ICQ_STR &text)
{
	// @FIXME: In case of race condition
	int i = bcMsgPos++;
	bcMsgPos = BCMSG_INDEX(bcMsgPos);
	BCMsg *msg = bcMsgQueue + i;
	DBRequest *req;

	if (msg->id) {
		req = new DBRequest(DBF_UPDATE);
		WRITE_STR(req, "DELETE FROM bcmsg_tbl WHERE id=");
		*req << msg->id;
		DBManager::query(req);
	}

	msg->id = id;
	msg->type = type;
	msg->when = curTime;
	msg->text.len = text.len;
	memcpy(msg->buf, text.text, text.len + 1);

	return i;
}

// Callbacks...

static void onlineNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	Session *s = (UdpSession *) req->refObj;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		uint32 uin = atol(row[0]);
		UdpSession *session = SessionHash::get(uin);
		if (session)
			session->sendOnline(s, emptyStr);
	}
}

static void offlineNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	Session *s = (UdpSession *) req->refObj;
	uint32 uin = s->uin;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		UdpSession *session = SessionHash::get(atol(row[0]));
		if (session)
			session->sendOffline(uin, emptyStr);
	}
}

static void statusNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *s = (UdpSession *) req->refObj;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		uint32 uin = atol(row[0]);
		UdpSession *session = SessionHash::get(uin);
		if (session)
			session->sendStatusChanged(s, emptyStr);
	}
}

static void remoteOnlineNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		Server *server = Server::createServer(row[0]);
		server->sendUserOnline(session);
	}
}

static void remoteOfflineNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;
	uint32 uin = session->uin;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		Server *server = Server::createServer(row[0]);
		server->sendUserOffline(uin);
	}
}

static void remoteStatusNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;
	uint32 uin = session->uin;
	uint32 status = session->status;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		Server *server = Server::createServer(row[0]);
		server->sendUserStatus(uin, status);
	}
}

static void newUserCB(DBRequest *req)
{
	UdpSession *session = (UdpSession *) req->refObj;
	UdpOutPacket *out = session->createPacket(UDP_NEW_UIN, req->data);

	uint32 uin = 0;
	if (req->ret == 0) {
		uin = req->lastInsertID;

		DBRequest *req = new DBRequest(DBF_UPDATE);
		WRITE_STR(req, "INSERT INTO ext_tbl (uin) VALUES(");
		*req << uin << ')';
		DBManager::query(req);
	}
	session->uin = uin;

	LOG(2) ("%lu registered\n", uin);

	*out << uin;
	*out << _ops.domain.c_str();
	session->sendPacket(out);
}

static void contactListCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;
	UdpOutPacket *out = session->createPacket(UDP_GET_CONTACTLIST, req->data);

	uint16 n = (uint16) mysql_num_rows(req->res);
	*out << n;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		uint32 uin = atol(row[0]);
		*out << uin;
	}
	session->sendPacket(out);
}

static void remoteContactListCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;
	UdpOutPacket *out = session->createPacket(UDP_GET_REMOTE_CONTACTLIST, req->data);

	*out << _ops.domain.c_str();

	uint16 n = (uint16) mysql_num_rows(req->res);
	*out << n;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		uint32 *lengths = mysql_fetch_lengths(req->res);
		uint32 uin = atol(row[0]);

		*out << uin;
		out->writeString(row[1], lengths[1]);
	}

	session->sendPacket(out);
}

static void offlineMsgCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;
	
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		uint32 *lengths = mysql_fetch_lengths(req->res);

		uint8 type = atoi(row[0]);
		QID src;
		src.uin = atol(row[1]);
		src.domain.text = row[2];
		src.domain.len = lengths[2];
		uint32 when = atol(row[3]);
		ICQ_STR text = { row[4], lengths[4] };

		session->sendMessage(type, src, when, text);
	}

	if (mysql_num_rows(req->res)) {
		DBRequest *req = new DBRequest(DBF_UPDATE);
		WRITE_STR(req, "DELETE FROM message_tbl WHERE dst=");
		*req << session->uin;
		DBManager::query(req);
	}
}

static void offlineBCMsgCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		uint32 *lengths = mysql_fetch_lengths(req->res);

		uint32 id = atol(row[0]);
		uint8 type = atoi(row[1]);
		uint32 when = atol(row[2]);
		ICQ_STR text;
		text.text = row[3];
		text.len = lengths[3];

		session->sendMessage(type, qidAdmin, when, text);
		if (session->lastMsgID < id)
			session->lastMsgID = id;
	}
}

static void loginFriendCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;
	UdpOutPacket *out = NULL;
	uint16 num = 0;
	uint8 *old = NULL;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		uint32 uin = atol(row[0]);
		UdpSession *s = SessionHash::get(uin);
		if (s && s->status != STATUS_INVIS) {
			if (!num) {
				out = session->createPacket(UDP_SRV_MULTI_ONLINE);
				old = out->skip(sizeof(num));
			}

			*out << uin << s->status;
			out->write32(s->ip);
			out->write16(s->msgport);
			out->write32(s->realip);

			if (++num >= MAX_ONLINES_PER_PACKET) {
				old = out->setCursor(old);
				*out << num;
				out->setCursor(old);
				session->sendPacket(out);

				num = 0;
			}
		}
	}

	if (num) {
		old = out->setCursor(old);
		*out << num;
		out->setCursor(old);
		session->sendPacket(out);
	}
}

static void remoteFriendCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		Server *server = Server::getServer(row[1]);
		if (!server)
			continue;

		Session *s = server->getSession(atol(row[0]));
		if (s)
			session->sendOnline(s, server->domain);
	}
}

static void loginVerifyCB(DBRequest *req)
{
	UdpSession *session = (UdpSession *) req->refObj;

	uint32 uin = session->uin;
	uint8 error = LOGIN_INVALID_UIN;
	if (req->res) {
		MYSQL_ROW row = mysql_fetch_row(req->res);
		if (row) {
			const char *passwd = row[0];
			session->face = atoi(row[1]);
			strncpy(session->nickname, row[2], MAX_NICK);
			strncpy(session->province, row[3], MAX_PROVINCE);
			session->auth = atoi(row[4]);
			session->oldMsgID = session->lastMsgID = atol(row[5]);

			error = LOGIN_SUCCESS;

			// Create the 128-bit DES subkey
			char pass[8];
			strncpy(pass, passwd, 8);
			des_setkey(session->subkey, pass);
		}
	}

	UdpOutPacket *out = session->createPacket(UDP_LOGIN, req->data);
	*out << error;

	if (error != LOGIN_SUCCESS) {
		LOG(3) ("%lu login failed\n", uin);
		session->status = STATUS_OFFLINE;
	} else {
		out->write32(session->ip);
		*out << UdpSession::sessionCount;
	}
	session->sendPacket(out);

	if (session->status == STATUS_OFFLINE)
		return;

	UdpSession *s = SessionHash::get(uin);
	if (s != session) {
		if (s) {
			LOG(4) ("%lu already logged in\n", uin);
			s->dead();
		} else
			UdpSession::sessionCount++;

		// Until now, we have successfully logged in
		LOG(2) ("%lu logged in from %s\n", uin, myicq_inet_ntoa(session->ip));

		SessionHash::put(session, uin);
	}

	// Inform me of my friends' status.
	req = new DBRequest(0, loginFriendCB, session);
	WRITE_STR(req, "SELECT uin2 FROM friend_tbl WHERE uin1=");
	*req << uin;
	WRITE_STR(req, LIMIT_FRIENDS);
	DBManager::query(req);

	// Remote friends
	if (_ops.enableS2S) {
		req = new DBRequest(0, remoteFriendCB, session);
		WRITE_STR(req, "SELECT uin2, domain FROM remote_friend_tbl WHERE uin1=");
		*req << uin;
		WRITE_STR(req, LIMIT_FRIENDS);
		DBManager::query(req);
	}

	// Online notify
	if (session->status != STATUS_INVIS)
		session->onlineNotify();

	// Offline messages
	req = new DBRequest(0, offlineMsgCB, session);
	WRITE_STR(req, "SELECT type, src, domain, time, text FROM message_tbl WHERE dst=");
	*req << uin;
	DBManager::query(req);

	// Broadcast messages
	if (uin != ADMIN_UIN) {
		req = new DBRequest(0, offlineBCMsgCB, session);
		WRITE_STR(req, "SELECT id, type, time, text FROM bcmsg_tbl WHERE id>");
		*req << session->lastMsgID;
		DBManager::query(req);
	}

	session->sendGroupTypes();
}

static void updateContactCB(DBRequest *req)
{
	MYSQL_ROW row;
	if (!req->res || !(row = mysql_fetch_row(req->res)))
		return;

	Server *server = (Server *) req->refObj;
	uint16 seq = req->data;
	uint32 src = atol(row[0]);

	uint32 *lengths = mysql_fetch_lengths(req->res);

	IcqOutPacket p;

	p << (uint32) atol(row[1]);			// uin
	p << (uint8) atoi(row[2]);			// face
	p.writeString(row[3], lengths[3]);	// nick
	p << (uint8) atoi(row[4]);			// age
	p << (uint8) atoi(row[5]);			// gender

	p.writeString(row[6], lengths[6]);	// country
	p.writeString(row[7], lengths[7]);	// province
	p.writeString(row[8], lengths[8]);	// city

	p.writeString(row[9], lengths[9]);	// email
	p.writeString(row[10], lengths[10]);	// address
	p.writeString(row[11], lengths[11]);	// zipcode
	p.writeString(row[12], lengths[12]);	// tel

	p.writeString(row[13], lengths[13]);	// name
	p << (uint8) atoi(row[14]);			// blood
	p.writeString(row[15], lengths[15]);	// college
	p.writeString(row[16], lengths[16]);	// profession
	p.writeString(row[17], lengths[17]);	// homepage
	p.writeString(row[18], lengths[18]);	// intro

	if (!server) {
		UdpSession *s = SessionHash::get(src);
		if (s)
			s->updateContactReply(seq, p.getData(), p.getLength());
	} else
		server->updateContactReply(seq, src, p.getData(), p.getLength());
}

static void updateUserCB(DBRequest *req)
{
	MYSQL_ROW row;
	if (!req->res || !(row = mysql_fetch_row(req->res)))
		return;

	UdpSession *session = (UdpSession *) req->refObj;
	UdpOutPacket *out = session->createPacket(UDP_UPDATE_USER, req->data);

	uint32 *lengths = mysql_fetch_lengths(req->res);

	*out << (uint8) atoi(row[0]);			// face
	out->writeString(row[1], lengths[1]);	// nick
	*out << (uint8) atoi(row[2]);			// age
	*out << (uint8) atoi(row[3]);			// gender

	out->writeString(row[4], lengths[4]);	// country
	out->writeString(row[5], lengths[5]);	// province
	out->writeString(row[6], lengths[6]);	// city

	out->writeString(row[7], lengths[7]);	// email
	out->writeString(row[8], lengths[8]);	// address
	out->writeString(row[9], lengths[9]);	// zipcode
	out->writeString(row[10], lengths[10]);	// tel

	out->writeString(row[11], lengths[11]);	// name
	*out << (uint8) atoi(row[12]);			// blood
	out->writeString(row[13], lengths[13]);	// college
	out->writeString(row[14], lengths[14]);	// profession
	out->writeString(row[15], lengths[15]);	// homepage
	out->writeString(row[16], lengths[16]);	// intro

	*out << session->auth;

	session->sendPacket(out);
}

static void searchResultCB(DBRequest *req)
{
	if (!req->res)
		return;

	UdpSession *session = (UdpSession *) req->refObj;
	UdpOutPacket *out = session->createPacket(UDP_SRV_SEARCH);

	uint16 num = 0;
	uint8 *old = out->skip(sizeof(num));

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		uint32 *lengths = mysql_fetch_lengths(req->res);

		uint32 uin = atoi(row[0]);
		uint8 online = !!SessionHash::get(uin);

		*out << uin << online;
		*out << (uint8) atoi(row[1]);			// face
		out->writeString(row[2], lengths[2]);	// nick
		out->writeString(row[3], lengths[3]);	// province
		num++;
	}

	old = out->setCursor(old);
	*out << num;
	out->setCursor(old);
	session->sendPacket(out);
}

static void searchUINCB(DBRequest *req)
{
	MYSQL_ROW row;
	if (!req->res || !(row = mysql_fetch_row(req->res)))
		return;

	Server *server = (Server *) req->refObj;
	uint16 seq = req->data;

	uint32 *lengths = mysql_fetch_lengths(req->res);

	uint32 src = atol(row[0]);
	uint32 dst = atol(row[1]);
	uint8 face = atoi(row[2]);

	IcqOutPacket out;
	out << dst << (uint8) 0;
	out << face;
	out.writeString(row[3], lengths[3]);
	out.writeString(row[4], lengths[4]);

	if (server)
		server->searchUINReply(seq, src, out.getData(), out.getLength());
	else {
		UdpSession *s = SessionHash::get(src);
		if (s)
			s->searchUINReply(seq, out.getData(), out.getLength());
	}
}

static void queryAuthCB(DBRequest *req)
{
	MYSQL_ROW row;
	if (!req->res || !(row = mysql_fetch_row(req->res)))
		return;

	Server *server = (Server *) req->refObj;
	uint16 seq = req->data;

	QID dst;
	dst.domain = emptyStr;

	uint32 src = atol(row[0]);
	dst.uin = atol(row[1]);
	uint8 auth = atoi(row[2]);

	UdpSession::addFriendAuth(seq, dst, src, server, auth);
}

static void insertBCMsgCB(DBRequest *req)
{
	if (req->ret != 0)
		return;

	BCMsg *msg = bcMsgQueue + req->data;
	msg->id = req->lastInsertID;
}

bool UdpSession::init()
{
	sockaddr_in addr;

	qidAdmin.uin = ADMIN_UIN;
	qidAdmin.domain = emptyStr;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		LOG(1) ("socket() failed\n");
		goto failed;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = _ops.myicqIP;
	addr.sin_port = _ops.myicqPort;
	if (bind(sock, (sockaddr *) &addr, sizeof(addr)) < 0) {
		LOG(1) ("bind(): Can not bind on %s:%d\n",
			inet_ntoa(addr.sin_addr), ntohs(_ops.myicqPort));
		goto failed;
	}

	desinit(0);
	return true;

failed:
	if (sock >= 0)
		close(sock);
	return false;
}

void UdpSession::destroy()
{
	ListHead *pos, *head = &keepAliveList;
	while ((pos = head->next) != head) {
		UdpSession *p = LIST_ENTRY(pos, UdpSession, listItem);
		pos->remove();
		delete p;
	}

	if (sock >= 0)
		close(sock);
}

void UdpSession::addFriendAuth(uint16 seq, QID &dst, uint32 src, Server *server,
							   uint8 auth, bool sendAuthMsg)
{
	if (server)
		server->addFriendReply(seq, dst.uin, src, auth);
	else {
		UdpSession *s = SessionHash::get(src);
		if (s) {
			UdpOutPacket *out = s->createPacket(UDP_ADD_FRIEND, seq);
			*out << dst.uin << auth << dst.domain;
			s->sendPacket(out);
		}
	}

	if (auth == ADD_FRIEND_ACCEPTED) {
		QID qid;
		qid.uin = src;
		qid.domain = (server ? server->domain : emptyStr);

		if (sendAuthMsg)
			UdpSession::sendMessage(MSG_ADDED, dst, qid, curTime, emptyStr);

		dbAddFriend(dst, qid);
	}
}

void UdpSession::addFriend(uint16 seq, QID &dst, uint32 src, Server *server)
{
	if (!dst.domain.len) {
		UdpSession *s = SessionHash::get(dst.uin);
		if (s)
			addFriendAuth(seq, dst, src, server, s->auth);
		else {
			DBRequest *req = new DBRequest(0, queryAuthCB, server, seq);
			WRITE_STR(req, "SELECT ");
			*req << src;
			WRITE_STR(req, ", uin, auth FROM basic_tbl WHERE uin=");
			*req << dst.uin;
			DBManager::query(req);
		}
	} else {
		server = Server::createServer(dst.domain.text);
		server->addFriend(seq, dst.uin, src);
	}
}

void UdpSession::sendMessage(uint8 type, QID &dst, QID &src, uint32 when, ICQ_STR &text)
{
	if (dst.domain.len) {
		Server *server = Server::createServer(dst.domain.text);
		server->sendMessage(type, dst.uin, src.uin, text);

	} else {
		UdpSession *s = SessionHash::get(dst.uin);
		if (s)
			s->sendMessage(type, src, when, text);
		else {
			DBRequest *req = new DBRequest(DBF_UPDATE);

			// @FIXME: Insert only when dst uin exists!
			WRITE_STR(req, "INSERT INTO message_tbl values(");
			*req << dst.uin << ',' << src.uin;
			*req << ',' << type << ',' << when << ',' << text;
			*req << ',' << src.domain << ')';
			DBManager::query(req);
		}
	}

	if (type == MSG_AUTH_ACCEPTED)
		dbAddFriend(src, dst);
}

void UdpSession::updateContact(uint16 seq, uint32 dst, uint32 src, Server *server)
{
	DBRequest *req = new DBRequest(0, updateContactCB, server, seq);
	WRITE_STR(req, "SELECT ");
	*req << src;
	WRITE_STR(req, ", a.uin, pic, nick, age, gender, country, province, city, email,"
		"address, zipcode, tel, name, blood, college, profession, homepage, intro "
		"FROM basic_tbl a, ext_tbl b WHERE a.uin=b.uin AND a.uin=");
	*req << dst;

	DBManager::query(req);
}

void UdpSession::updateContactReply(uint16 seq, uint8 *data, int n, Server *server)
{
	UdpOutPacket *out = createPacket(UDP_UPDATE_CONTACT, seq);

	out->writeData(data, n);
	if (server)
		*out << server->domain;

	sendPacket(out);
}

void UdpSession::searchRandomReply(uint16 seq, uint8 *data, int n)
{
	UdpOutPacket *out = createPacket(UDP_SRV_SEARCH, seq);
	out->writeData(data, n);
	sendPacket(out);
}

void UdpSession::searchUINReply(uint16 seq, uint8 *data, int n)
{
	UdpOutPacket *out = createPacket(UDP_SRV_SEARCH, seq);
	*out << (uint16) 1;		// Only one result
	out->writeData(data, n);
	sendPacket(out);
}

bool UdpSession::onReceive()
{
	char buf[UDP_PACKET_SIZE];
	sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int n = recvfrom(sock, buf, UDP_PACKET_SIZE, 0, (sockaddr *) &addr, &len);

	if (n < (int) sizeof(UDP_CLI_HDR))
		return false;

	UdpInPacket in(buf, n);

	uint32 ip = addr.sin_addr.s_addr;
	uint16 port = addr.sin_port;
	UdpSession *s = SessionHash::get(ip, port);

	if (!s) {
		uint16 cmd = in.header.cmd;
		if (cmd == UDP_NEW_UIN || cmd == UDP_LOGIN) {
			s = new UdpSession(in, ip, port);

			// Add it to the hash
			SessionHash::put(s, ip, port);
			keepAliveList.add(&s->listItem);
		} else
			s = SessionHash::get(in.header.uin);
	}
	if (s)
		s->onPacketReceived(in);

	return true;
}

void UdpSession::checkSendQueue()
{
	ListHead *pos, *head = &globalSendQueue;

	time_t now = curTime;
	while ((pos = head->next) != head) {
		UdpOutPacket *p = LIST_ENTRY(pos, UdpOutPacket, globalSendItem);
		if (p->expire > now)
			break;

		pos->remove();
		p->attempts++;

		if (p->attempts <= MAX_SEND_ATTEMPTS) {
			// Resend packet
			LOG(4) ("Packet %d timeout, resend it\n", p->seq);
			p->session->sendDirect(p);
			p->expire = now + SEND_TIMEOUT;
			globalSendQueue.add(pos);
		} else {
			// Maximum attempts reached, delete it!
			LOG(4) ("Packet %d timeout, delete it\n", p->seq);
			p->sendItem.remove();
			delete p;
		}
	}
}

void UdpSession::checkKeepAlive()
{
	time_t now = curTime;
	ListHead *pos, *head = &keepAliveList;

	while ((pos = head->next) != head) {
		UdpSession *p = LIST_ENTRY(pos, UdpSession, listItem);
		if (p->expire > now)
			break;

		LOG(2) ("%lu logged out abnormally\n", p->uin);
		p->logout();
	}
}

UdpSession::UdpSession(UdpInPacket &in, uint32 ip, uint16 port)
{
	udpver = in.header.ver;
	sid = in.header.sid;
	recvSeq = sendSeq = in.header.seq;
	this->ip = ip;
	this->port = port;

	uin = 0;
	auth = 0;
	face = 0;
	nickname[MAX_NICK] = '\0';
	province[MAX_PROVINCE] = '\0';

	tcpver = 0;
	realip = 0;
	msgport = 0;
	status = STATUS_OFFLINE;
	window = 0;
	lastMsgID = 0;

	isDead = 0;

	group = NULL;

	expire = curTime + KEEPALIVE_TIMEOUT;
}

UdpSession::~UdpSession()
{
	ListHead *head = &sendQueue;
	ListHead *pos;

	while ((pos = head->next) != head) {
		UdpOutPacket *p = LIST_ENTRY(pos, UdpOutPacket, sendItem);
		pos->remove();
		p->globalSendItem.remove();
		delete p;
	}
}


void UdpSession::dead()
{
	if (group) {
		LOG(4) ("%lu exited group %d\n", uin, group->id);
		group->removeMember(this);
		group = NULL;
	}

	uinItem.remove();
	ipportItem.remove();
	listItem.remove();

	release();
}

inline void UdpSession::notify(DB_CALLBACK cb1, DB_CALLBACK cb2)
{
	DBRequest *req = new DBRequest(0, cb1, this);
	WRITE_STR(req, "SELECT uin1 FROM friend_tbl WHERE uin2=");
	*req << uin;
	WRITE_STR(req, LIMIT_FRIENDS);
	DBManager::query(req);

	if (_ops.enableS2S) {
		req = new DBRequest(0, cb2, this);
		WRITE_STR(req, "SELECT DISTINCT domain FROM notify_tbl WHERE uin1=");
		*req << uin;
		DBManager::query(req);
	}
}

void UdpSession::onlineNotify()
{
	notify(onlineNotifyCB, remoteOnlineNotifyCB);
}

void UdpSession::offlineNotify()
{
	notify(offlineNotifyCB, remoteOfflineNotifyCB);
}

void UdpSession::statusNotify(uint32 newStatus)
{
	if (status == newStatus)
		return;

	uint32 oldStatus = status;
	status = newStatus;

	if (oldStatus == STATUS_INVIS && status != STATUS_INVIS)
		onlineNotify();
	else if (oldStatus != STATUS_INVIS && status == STATUS_INVIS)
		offlineNotify();
	else if (status != STATUS_INVIS)
		notify(statusNotifyCB, remoteStatusNotifyCB);
}

void UdpSession::createPacket(UdpOutPacket &out, uint16 cmd, uint16 seq, uint16 ackseq)
{
	out << udpver << (uint32) 0 << uin << sid << cmd << seq << ackseq;
}

UdpOutPacket *UdpSession::createPacket(uint16 cmd, uint16 ackseq)
{
	UdpOutPacket *out = new UdpOutPacket(this);
	out->seq = ++sendSeq;
	createPacket(*out, cmd, out->seq, ackseq);
	return out;
}

inline void UdpSession::sendDirect(UdpOutPacket *p)
{
	p->send(sock, ip, port);
}

void UdpSession::sendAckPacket(uint16 seq)
{
	UdpOutPacket out;
	createPacket(out, UDP_ACK, 0, seq);
	sendDirect(&out);
}

void UdpSession::sendPacket(UdpOutPacket *p)
{
	p->attempts = 0;
	p->expire = curTime + SEND_TIMEOUT;

	sendDirect(p);
	sendQueue.add(&p->sendItem);
	globalSendQueue.add(&p->globalSendItem);
}

bool UdpSession::setWindow(uint16 seq)
{
	const int SEQ_WIN_SIZE = 32;

	int d = seq - recvSeq;
	if (d < 0)
		return false;

	if (d == SEQ_WIN_SIZE) {
		do {
			++recvSeq;
			window >>= 1;
		} while (window & 0x01);

		d = seq - recvSeq;
	}

	if (!d) {
		do {
			++recvSeq;
			window >>= 1;
		} while (window & 0x01);

		return true;
	}

	uint32 mask = (1 << d);
	if (window & mask)
		return false;

	window |= mask;
	return true;
}

void UdpSession::sendOnline(Session *s, ICQ_STR &domain)
{
	UdpOutPacket *out = createPacket(UDP_SRV_USER_ONLINE);

	*out << s->uin << s->status;
	out->write32(s->ip);
	out->write16(s->msgport);
	out->write32(s->realip);
	*out << domain;

	sendPacket(out);
}

void UdpSession::sendOffline(uint32 uin, ICQ_STR &domain)
{
	UdpOutPacket *out = createPacket(UDP_SRV_USER_OFFLINE);
	*out << uin << domain;
	sendPacket(out);
}

void UdpSession::sendStatusChanged(Session *s, ICQ_STR &domain)
{
	UdpOutPacket *out = createPacket(UDP_SRV_STATUS_CHANGED);
	*out << s->uin << s->status << domain;
	sendPacket(out);
}

void UdpSession::sendMessage(uint8 type, QID &src, uint32 when, ICQ_STR &text)
{
	UdpOutPacket *out = createPacket(UDP_SRV_MESSAGE);
	*out << type << src.uin << when << text << src.domain;
	sendPacket(out);
}

void UdpSession::logout()
{
	if (status != STATUS_OFFLINE) {
		if (status != STATUS_INVIS)
			offlineNotify();
		--sessionCount;

		// Record the last msg_id
		if (oldMsgID < lastMsgID) {
			DBRequest *req = new DBRequest(DBF_UPDATE);
			WRITE_STR(req, "UPDATE basic_tbl SET msg_id=");
			*req << lastMsgID;
			WRITE_STR(req, " WHERE uin=");
			*req << uin;
			DBManager::query(req);
		}
	}

	dead();
}

bool UdpSession::onPacketReceived(UdpInPacket &in)
{
	uint16 cmd = in.header.cmd;
	uint16 seq = in.header.seq;

	if (cmd != UDP_LOGIN && cmd != UDP_NEW_UIN) {
		if (uin != in.header.uin || sid != in.header.sid) {
			// Packet does not belong to this session
			LOG(4) ("Packet does not belong to this session\n");
			return false;
		}
		if (!in.decrypt(subkey)) {
			LOG(4) ("Can not decrypt packet\n");
			return false;
		}
	}

	if (cmd == UDP_ACK) {
		onAck(seq);
		return true;
	}
	if (!setWindow(seq)) {
		// Packet is duplicated
		LOG(4) ("Packet %d (cmd = %d) is duplicated\n", seq, in.header.cmd);
		sendAckPacket(seq);
		return false;
	}

	switch (cmd) {
	case UDP_KEEPALIVE:
		onKeepAlive(in);
		break;

	case UDP_NEW_UIN:
		onNewUIN(in);
		break;

	case UDP_GET_CONTACTLIST:
		onGetContactList(in);
		break;

	case UDP_GET_REMOTE_CONTACTLIST:
		onGetRemoteContactList(in);
		break;

	case UDP_LOGIN:
		onLogin(in);
		break;

	case UDP_LOGOUT:
		onLogout(in);
		break;

	case UDP_CHANGE_STATUS:
		onChangeStatus(in);
		break;

	case UDP_UPDATE_CONTACT:
		onUpdateContact(in);
		break;

	case UDP_UPDATE_USER:
		onUpdateUser(in);
		break;

	case UDP_MODIFY_USER:
		onModifyUser(in);
		break;

	case UDP_SEND_MSG:
		onSendMessage(in);
		break;

	case UDP_SEARCH_RANDOM:
		onSearchRandom(in);
		break;

	case UDP_SEARCH_CUSTOM:
		onSearchCustom(in);
		break;

	case UDP_ADD_FRIEND:
		onAddFriend(in);
		break;

	case UDP_DEL_FRIEND:
		onDelFriend(in);
		break;

	case UDP_BROADCAST_MSG:
		onSendBCMsg(in);
		break;

	case UDP_GET_SERVER_LIST:
		onGetServerList(in);
		break;

	case UDP_GET_GROUP_LIST:
		onGetGroupList(in);
		break;

	case UDP_SEARCH_GROUP:
		onSearchGroup(in);
		break;

	case UDP_CREATE_GROUP:
		onCreateGroup(in);
		break;

	case UDP_ENTER_GROUP:
		onEnterGroup(in);
		break;

	case UDP_EXIT_GROUP:
		onExitGroup(in);
		break;

	case UDP_GROUP_START:
		onGroupStart(in);
		break;

	case UDP_GROUP_MESSAGE:
		onGroupMessage(in);
		break;

	case UDP_GROUP_CMD:
		onGroupCmd(in);
		break;

	default:
		// Unknown UDP command
		LOG(4) ("Unknown command from client");
		return false;
	}
	return true;
}

void UdpSession::onAck(uint16 seq)

{
	ListHead *pos, *head = &sendQueue;

	LIST_FOR_EACH(pos, head) {
		UdpOutPacket *p = LIST_ENTRY(pos, UdpOutPacket, sendItem);
		if (p->seq != seq)
			continue;

		// The specified packet is found, delete it
		LOG(4) ("Packet %d is acked\n", seq);
		pos->remove();
		p->globalSendItem.remove();
		delete p;

		// We are here only waiting for an ACK packet
		if (isDead)
			dead();

		return;
	}

	LOG(4) ("Ack packet %d is ignored\n", seq);
}

void UdpSession::onNewUIN(UdpInPacket &in)
{
	if (_ops.enableRegister) {
		ICQ_STR passwd;
		in >> passwd;

		DBRequest *req = new DBRequest(DBF_UPDATE | DBF_INSERT, newUserCB, this, in.header.seq);
		WRITE_STR(req, "INSERT INTO basic_tbl (passwd, msg_id) SELECT PASSWORD(");
		*req << passwd;
		WRITE_STR(req, "), MAX(id) FROM bcmsg_tbl");
		DBManager::query(req);

	} else {
		UdpOutPacket *out = createPacket(UDP_NEW_UIN, in.header.seq);
		*out << (uint32) 0;
		sendPacket(out);
	}

	isDead = 1;
}

void UdpSession::onGetContactList(UdpInPacket &in)
{
	DBRequest *req = new DBRequest(0, contactListCB, this, in.header.seq);
	WRITE_STR(req, "SELECT uin2 FROM friend_tbl WHERE uin1=");
	*req << uin;
	WRITE_STR(req, LIMIT_FRIENDS);
	DBManager::query(req);
}

void UdpSession::onGetRemoteContactList(UdpInPacket &in)
{
	DBRequest *req = new DBRequest(0, remoteContactListCB, this, in.header.seq);
	WRITE_STR(req, "SELECT uin2, domain FROM remote_friend_tbl WHERE uin1=");
	*req << uin;
	WRITE_STR(req, LIMIT_FRIENDS);
	DBManager::query(req);
}

void UdpSession::onLogin(UdpInPacket &in)
{
	if (status != STATUS_OFFLINE)
		return;

	ICQ_STR passwd;

	uin = in.header.uin;
	in >> passwd >> status >> tcpver;
	realip = in.read32();
	msgport = in.read16();

	DBRequest *req = new DBRequest(0, loginVerifyCB, this, in.header.seq);
	WRITE_STR(req, "SELECT ");
	*req << passwd;
	WRITE_STR(req, ", pic, nick, province, auth, msg_id FROM basic_tbl WHERE uin=");
	*req << uin;
	WRITE_STR(req, " AND passwd=password(");
	*req << passwd << ')';

	DBManager::query(req);
}

void UdpSession::onUpdateContact(UdpInPacket &in)
{
	QID dst;
	in >> dst.uin >> dst.domain;

	if (!dst.domain.len)
		updateContact(in.header.seq, dst.uin, uin);
	else if (_ops.enableS2S) {
		strLowerCase(dst.domain.text);
		Server *s = Server::createServer(dst.domain.text);
		s->updateContact(in.header.seq, dst.uin, uin);
	} else
		sendAckPacket(in.header.seq);
}

void UdpSession::onUpdateUser(UdpInPacket &in)
{
	DBRequest *req = new DBRequest(0, updateUserCB, this, in.header.seq);
	WRITE_STR(req, "SELECT pic, nick, age, gender, country, province, city, email,"
		"address, zipcode, tel, name, blood, college, profession, homepage, intro "
		"FROM basic_tbl a, ext_tbl b WHERE a.uin=b.uin AND a.uin=");
	*req << uin;

	DBManager::query(req);
}

void UdpSession::onModifyUser(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	ICQ_STR nick, country, prov, city;
	ICQ_STR email, address, zipcode, tel;
	ICQ_STR name, college, profession, homepage, intro;
	uint8 age, gender, blood, modifyPasswd;

	in >> face >> nick >> age >> gender >> country >> prov >> city;
	in >> email >> address >> zipcode >> tel;
	in >> name >> blood >> college >> profession >> homepage >> intro;
	in >> auth >> modifyPasswd;

	if (nick.len <= MAX_NICK)
		memcpy(nickname, nick.text, nick.len + 1);
	if (prov.len <= MAX_PROVINCE)
		memcpy(province, prov.text, prov.len + 1);

	// Update basic info
	DBRequest *req = new DBRequest(DBF_UPDATE);
	WRITE_STR(req, "UPDATE basic_tbl SET ");
	if (modifyPasswd) {
		ICQ_STR passwd;
		in >> passwd;
		if (passwd.len) {
			WRITE_STR(req, "passwd=password(");
			*req << passwd << ')' << ',';
		}
	}
	WRITE_STR(req, "nick=");
	*req << nick;
	WRITE_STR(req, ",pic=");
	*req << face;
	WRITE_STR(req, ",gender=");
	*req << gender;
	WRITE_STR(req, ",age=");
	*req << age;
	WRITE_STR(req, ",country=");
	*req << country;
	WRITE_STR(req, ",province=");
	*req << prov;
	WRITE_STR(req, ",city=");
	*req << city;
	WRITE_STR(req, ",email=");
	*req << email;
	WRITE_STR(req, ",auth=");
	*req << auth;
	WRITE_STR(req, " WHERE uin=");
	*req << uin;

	DBManager::query(req);

	// Update ext info
	req = new DBRequest(DBF_UPDATE);
	WRITE_STR(req, "UPDATE ext_tbl SET address=");
	*req << address;
	WRITE_STR(req, ",zipcode=");
	*req << zipcode;
	WRITE_STR(req, ",tel=");
	*req << tel;
	WRITE_STR(req, ",name=");
	*req << name;
	WRITE_STR(req, ",blood=");
	*req << blood;
	WRITE_STR(req, ",college=");
	*req << college;
	WRITE_STR(req, ",profession=");
	*req << profession;
	WRITE_STR(req, ",homepage=");
	*req << homepage;
	WRITE_STR(req, ",intro=");
	*req << intro;
	WRITE_STR(req, "WHERE uin=");
	*req << uin;

	DBManager::query(req);
}

void UdpSession::onLogout(UdpInPacket &in)
{
	LOG(2) ("%lu logged out\n", uin);
	logout();
}

void UdpSession::onChangeStatus(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	uint32 newStatus;
	in >> newStatus;
	statusNotify(newStatus);
}

void UdpSession::onKeepAlive(UdpInPacket &in)
{
	listItem.remove();
	expire = curTime + KEEPALIVE_TIMEOUT;
	keepAliveList.add(&listItem);

	UdpOutPacket out;
	createPacket(out, UDP_KEEPALIVE, ++sendSeq, in.header.seq);
	out << sessionCount;
	sendDirect(&out);

	// Broadcast system message
	if (uin == ADMIN_UIN)
		return;

	int i = BCMSG_INDEX(bcMsgPos - 1);
	BCMsg *msg = bcMsgQueue + i;
	uint32 id = msg->id;
	if (id > lastMsgID) {
		do {
			sendMessage(msg->type, qidAdmin, msg->when, msg->text);

			i = BCMSG_INDEX(i - 1);
			msg = bcMsgQueue + i;
		} while (i != bcMsgPos && msg->id > lastMsgID);

		lastMsgID = id;
	}
}

void UdpSession::onSendMessage(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	uint8 type;
	QID dst, src;
	ICQ_STR text;

	in >> type >> dst.uin >> text >> dst.domain;

	if (text.len > MAX_MSG_LEN) {
		LOG(4) ("Invalid message\n");
		return;
	}

	if (dst.domain.len && !_ops.enableS2S)
		return;

	strLowerCase(dst.domain.text);

	src.uin = uin;
	src.domain = emptyStr;
	sendMessage(type, dst, src, curTime, text);
}

void UdpSession::onSearchRandom(UdpInPacket &in)
{
	ICQ_STR domain;
	in >> domain;

	if (!domain.len) {
		UdpOutPacket *out = createPacket(UDP_SRV_SEARCH, in.header.seq);
		SessionHash::random(*out, MAX_SEARCH);
		sendPacket(out);

	} else if (_ops.enableS2S) {
		strLowerCase(domain.text);

		Server *server = Server::getServer(domain.text);
		if (server)
			server->searchRandom(in.header.seq, uin);
	} else
		sendAckPacket(in.header.seq);
}

void UdpSession::searchUIN(uint16 seq, uint32 dst, uint32 src, Server *server)
{
	UdpSession *s = SessionHash::get(dst);
	if (s) {
		IcqOutPacket out;
		out << dst;
		out << (uint8) (s->status == STATUS_INVIS ? 0 : 1);	// online/offline
		out << s->face;
		out << s->nickname;
		out << s->province;

		if (server)
			server->searchUINReply(seq, src, out.getData(), out.getLength());
		else {
			UdpSession *session = SessionHash::get(src);
			if (session)
				session->searchUINReply(seq, out.getData(), out.getLength());
		}
	} else {
		DBRequest *req = new DBRequest(0, searchUINCB, server, seq);
		WRITE_STR(req, "SELECT ");
		*req << src;
		WRITE_STR(req, ", uin, pic, nick, province FROM basic_tbl WHERE uin=");
		*req << dst;
		DBManager::query(req);
	}
}

void UdpSession::onSearchCustom(UdpInPacket &in)
{
	uint32 dst;
	in >> dst;

	if (dst) {
		ICQ_STR domain;
		in >> domain;

		if (!domain.len)
			searchUIN(in.header.seq, dst, uin);
		else if (_ops.enableS2S) {
			strLowerCase(domain.text);
			Server *s = Server::createServer(domain.text);
			s->searchUIN(in.header.seq, dst, uin);
		} else
			sendAckPacket(in.header.seq);

		return;
	}

	ICQ_STR nick, email;
	uint32 start;
	in >> nick >> email >> start;

	if (nick.len || email.len) {
		DBRequest *req = new DBRequest(0, searchResultCB, this);
		WRITE_STR(req, "SELECT uin, pic, nick, province FROM basic_tbl WHERE uin>");
		*req << start;
		WRITE_STR(req, " AND ");
		if (nick.len) {
			WRITE_STR(req, "nick=");
			*req << nick;
		}
		if (email.len) {
			if (nick.len)
				WRITE_STR(req, " AND ");
			WRITE_STR(req, "email=");
			*req << email;
		}
		WRITE_STR(req, LIMIT_SEARCH);

		DBManager::query(req);

	}
}

void UdpSession::onAddFriend(UdpInPacket &in)
{
	QID dst;
	in >> dst.uin >> dst.domain;

	if (dst.domain.len && !_ops.enableS2S)
		sendAckPacket(in.header.seq);
	else {
		strLowerCase(dst.domain.text);
		addFriend(in.header.seq, dst, uin);
	}
}

void UdpSession::onDelFriend(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	QID dst;
	in >> dst.uin >> dst.domain;

	DBRequest *req = new DBRequest(DBF_UPDATE);

	if (!dst.domain.len) {
		WRITE_STR(req, "DELETE FROM friend_tbl WHERE uin1=");
		*req << uin;
		WRITE_STR(req, " AND uin2=");
		*req << dst.uin;

	} else {
		if (_ops.enableS2S) {
			strLowerCase(dst.domain.text);
			Server *server = Server::createServer(dst.domain.text);
			server->delFriend(dst.uin, uin);
		}

		WRITE_STR(req, "DELETE FROM remote_friend_tbl WHERE uin1=");
		*req << uin;
		WRITE_STR(req, " AND uin2=");
		*req << dst.uin;
		WRITE_STR(req, " AND domain=");
		*req << dst.domain;
	}
	DBManager::query(req);
}

void UdpSession::onSendBCMsg(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	if (uin != ADMIN_UIN) {
		LOG(4) ("%lu has no privileges to broadcast a message\n");
		return;
	}

	uint8 type;
	ICQ_STR text;
	in >> type >> text;

	if (text.len > MAX_MSG_LEN)
		return;

	int i = addBCMsg(0, type, text);
	DBRequest *req = new DBRequest(DBF_UPDATE | DBF_INSERT, insertBCMsgCB, this, i);
	WRITE_STR(req, "INSERT INTO bcmsg_tbl VALUES(NULL,");
	*req << type << ',' << (uint32) curTime << ',' << text << ')';
	DBManager::query(req);
}

void UdpSession::onGetServerList(UdpInPacket &in)
{
	const int MAX_SERVERS = 25;

	UdpOutPacket *out = createPacket(UDP_GET_SERVER_LIST, in.header.seq);
	uint16 num = 0;
	uint8 *old = out->skip(sizeof(num));
	
	ListHead *head = &Server::serverList;
	ListHead *pos;

	LIST_FOR_EACH(pos, head) {
		Server *s = LIST_ENTRY(pos, Server, listItem);
		if (s->domain.len) {
			*out << s->domain << s->desc << s->sessionCount;
			if (++num >= MAX_SERVERS)
				break;
		}
	}

	old = out->setCursor(old);
	*out << num;
	out->setCursor(old);
	sendPacket(out);
}

void UdpSession::sendGroupTypes()
{
	int n = GroupPlugin::numPlugins;
	if (n <= 0)
		return;

	UdpOutPacket *out = createPacket(UDP_SRV_GROUP_TYPES);
	*out << (uint16) n;

	GroupPlugin **p = GroupPlugin::plugins;
	for (int i = 0; i < n; i++) {
		*out << p[i]->name.c_str();
		*out << p[i]->displayName.c_str();
	}

	sendPacket(out);
}

void UdpSession::onSearchGroup(UdpInPacket &in)
{
	uint32 id;
	in >> id;

	IcqGroup *g = GroupPlugin::getGroup(id);
	if (!g)
		return;

	uint16 type = g->plugin->type;
	UdpOutPacket *out = createPacket(UDP_SEARCH_GROUP, in.header.seq);
	*out << id;
	*out << g->name << type << (uint16) g->numMembers;

	sendPacket(out);
}

void UdpSession::onGetGroupList(UdpInPacket &in)
{
	uint16 type;
	in >> type;

	GroupPlugin *p = GroupPlugin::getPlugin(type);
	if (!p)
		return;

	UdpOutPacket *out = createPacket(UDP_GET_GROUP_LIST, in.header.seq);
	*out << (uint16) p->numGroups;

	ListHead *head = &p->groupList;
	ListHead *pos;
	LIST_FOR_EACH(pos, head) {
		IcqGroup *g = LIST_ENTRY(pos, IcqGroup, listItem);
		*out << g->id;
		*out << g->name << (uint16) g->numMembers;
	}
	sendPacket(out);
}

void UdpSession::onCreateGroup(UdpInPacket &in)
{
	uint32 id = 0;

	if (!group) {
		uint16 type;
		ICQ_STR name, pass;

		in >> type >> name >> pass;

		group = GroupPlugin::createGroup(type);
		if (group) {
			strncpy(group->name, name.text, MAX_GROUP_NAME_LEN);
			strncpy(group->pass, pass.text, MAX_GROUP_PASS_LEN);

			group->addMember(this);
			id = group->id;
		}
	}

	LOG(4) ("Group %lu is created\n", id);

	UdpOutPacket *out = createPacket(UDP_CREATE_GROUP, in.header.seq);
	*out << id;
	sendPacket(out);
}

void UdpSession::onEnterGroup(UdpInPacket &in)
{
	uint16 error;
	uint32 id;
	ICQ_STR pass;

	in >> id >> pass;

	UdpOutPacket *out = createPacket(UDP_ENTER_GROUP, in.header.seq);

	if (group) {
		if (group->id == id)
			error = GROUP_ERROR_ALREADY_EXIST;
		else
			error = GROUP_ERROR_EXCEED_MAX_GROUPS;
	} else {
		IcqGroup *g = GroupPlugin::getGroup(id);
		if (!g)
			error = GROUP_ERROR_NOT_EXIST;
		else if (strcmp(g->pass, pass.text))
			error = GROUP_ERROR_WRONG_PASSWD;
		else {
			if (g->addMember(this) < 0)
				error = GROUP_ERROR_EXCEED_MAX_MEMBERS;
			else {
				error = GROUP_ERROR_SUCCESS;
				group = g;
			}
		}
	}

	*out << id << error;

	if (error != GROUP_ERROR_SUCCESS) {
		sendPacket(out);
		return;
	}

	LOG(4) ("%lu entered group %d\n", uin, id);

	*out << (uint16) group->numMembers;

	int n = group->plugin->getMaxMembers();

	for (int i = 0; i < n; i++) {
		UdpSession *s = group->members[i];
		if (s) {
			*out << s->uin << s->face;
			*out << s->nickname;
		}
	}
	sendPacket(out);
}

void UdpSession::onExitGroup(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	if (!group)
		return;

	uint32 id;
	in >> id;

	if (group->id == id) {
		LOG(4) ("%lu exited group %d\n", uin, group->id);
		group->removeMember(this);
		group = NULL;
	}
}

void UdpSession::onGroupStart(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	uint32 id;
	in >> id;
	if (!group || group->id != id)
		return;

	group->start(uin);
}

void UdpSession::onGroupMessage(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	if (!group)
		return;

	uint32 id;
	ICQ_STR text;

	in >> id >> text;

	if (group->id == id)
		group->sendMessage(uin, curTime, text);
}

void UdpSession::onGroupCmd(UdpInPacket &in)
{
	sendAckPacket(in.header.seq);

	uint32 id;
	in >> id;
	if (!group || group->id != id)
		return;

	group->onPacketReceived(in);
}
