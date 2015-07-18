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
#include <iostream.h>
#include "udppacket.h"
#include "sessionhash.h"
#include "ndes.h"

enum {
	UDP_ACK = 1,
	UDP_NEW_UIN,
	UDP_GET_CONTACTLIST,
	UDP_LOGIN,
	UDP_LOGOUT,
	UDP_KEEPALIVE,
	UDP_CHANGE_STATUS,
	UDP_UPDATE_CONTACT,
	UDP_MODIFY_USER,
	UDP_UPDATE_USER,
	UDP_SEND_MSG,
	UDP_GROUP_SEND_MSG,
	UDP_SEARCH_RANDOM,
	UDP_SEARCH_CUSTOM,
	UDP_ADD_FRIEND,
	UDP_DEL_FRIEND,
	UDP_BROADCAST_MSG,

	UDP_SRV_USER_ONLINE = 0x0100,
	UDP_SRV_USER_OFFLINE,
	UDP_SRV_MULTI_ONLINE,
	UDP_SRV_STATUS_CHANGED,
	UDP_SRV_MESSAGE,
	UDP_SRV_SEARCH,
};

enum {
	LOGIN_SUCCESS,
	LOGIN_INVALID_UIN,
	LOGIN_WRONG_PASSWD,
};

enum {
	STATUS_ONLINE = 0,
	STATUS_OFFLINE,
	STATUS_AWAY,
	STATUS_INVIS
};

enum {
	MSG_TEXT,
	MSG_AUTO_REPLY,
	MSG_AUTH_ACCEPTED,
	MSG_AUTH_REQ,
	MSG_AUTH_REJECTED,
	MSG_ADDED,
	MSG_BROADCAST,
};

enum {
	ADD_FRIEND_ACCEPTED,
	ADD_FRIEND_AUTH_REQ,
	ADD_FRIEND_REJECTED,
};

#define MYICQ_UDP_VER		1
#define MYICQ_PORT_DEFAULT	8000
#define MAX_SEARCH_PER_PAGE	25
#define MAX_SEND_ATTEMPTS	2
#define MAX_SQL_STATEMENT	4096
#define MAX_MSG_LEN			512
#define START_UIN			1000

struct BROADCAST_MSG {
	IcqListItem listItem;

	uint32 id;
	uint8 type;
	uint32 when;
	uint32 src;
	char text[MAX_MSG_LEN];
	uint32 to;
	uint32 maxUIN;
	time_t expire;
};

int		UdpSession::sock = -1;
IcqList	UdpSession::globalSendQueue;
IcqList	UdpSession::keepAliveList;
IcqList UdpSession::broadMsgList;
MYSQL	UdpSession::mysql;
char	UdpSession::sqlStmt[MAX_SQL_STATEMENT];
uint32	UdpSession::sessionCount = 0;


bool UdpSession::initialize()
{
	srand(time(NULL));
	desinit(0);
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		cerr << "create socket failed." << endl;
		return false;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(MYICQ_PORT_DEFAULT);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (sockaddr *) &addr, sizeof(addr)) < 0) {
		cerr << "socket bind failed." << endl;
		close(sock);
		return false;
	}

	if (!mysql_init(&mysql)) {
		cerr << "mysql initialization failed." << endl;
		return false;
	}

	const char *host = LOCAL_HOST;
	const char *user = "myicq";
	const char *pass = "myicq";
	const char *db = "myicq";
	const char *unix_socket = NULL;

	if (!mysql_real_connect(&mysql, host, user, pass, db, 0, unix_socket, 0)) {
		cerr << "can not connect to mysql server." << endl;
		mysql_close(&mysql);
		return false;
	}

	return true;
}

void UdpSession::cleanUp()
{
	IcqListItem *head = &keepAliveList.head;
	IcqListItem *pos;
	UdpSession *p;

	while ((pos = head->next) != head) {
		p = LIST_ENTRY(pos, UdpSession, keepAliveItem);
		delete p;
	}

	mysql_close(&mysql);
	close(sock);
}

bool UdpSession::onReceive()
{
	UdpInPacket in;
	int n = in.recv(sock);
	if (n < 0)
		return false;

	if (n < (int) sizeof(UDP_HEADER)) {
		cout << "packet size is less than " << sizeof(UDP_HEADER) << endl;
		return false;
	}
	uint16 ver = in.getVersion();
	if (ver > MYICQ_UDP_VER) {
		cout << "packet v" << ver << " not supported." << endl;
		return false;
	}

	uint16 cmd = in.getCmd();
	uint32 uin = in.getUIN();
	uint32 ip = in.getIP();
	uint16 port = in.getPort();
	UdpSession *session = NULL;

	if (cmd == UDP_NEW_UIN)
		session = SessionHash::getDead(ip, port);
	else if (cmd != UDP_LOGIN) {
		session = SessionHash::getAlive(uin);
		if (!session && cmd == UDP_ACK)
			session = SessionHash::getDead(ip, port);
		if (!session)
			return false;
	}

	if (!session) {
		session = new UdpSession;
		if (!session) {
			cout << "create session failed." << endl;
			return false;
		}
	}

	session->onReceive(in);
	return true;
}

time_t UdpSession::checkSendQueue()
{
	IcqListItem *pos;
	UdpOutPacket *p;
	UdpSession *session;
	IcqListItem *head = &globalSendQueue.head;
	time_t now = time(NULL);

	while ((pos = head->next) != head) {
		p = LIST_ENTRY(head->next, UdpOutPacket, globalSendItem);
		if (p->expire > now)
			return (p->expire - now);

		session = p->session;
		cout << "packet " << p->getSeq() << " time out." << endl;


		p->attempts++;
		if (p->attempts <= MAX_SEND_ATTEMPTS) {
			cout << "retrasmit packet." << endl;
			pos->remove();
			p->expire = now + SEND_TIMEOUT;
			session->sendDirect(p);
			globalSendQueue.add(pos);
		}
		else {
			cout << "maximum attempts reached. delete it!" << endl;
			p->sendItem.remove();
			p->globalSendItem.remove();
			delete p;
		}
	}
	return SEND_TIMEOUT;
}

time_t UdpSession::checkKeepAlive()
{
	IcqListItem *pos;
	UdpSession *p;
	IcqListItem *head = &keepAliveList.head;
	time_t now = time(NULL);

	while ((pos = head->next) != head) {
		p = LIST_ENTRY(pos, UdpSession, keepAliveItem);
		if (p->expire > now)
			return (p->expire - now);

		cout << p->uin << " expires." << endl;
		if (p->status != STATUS_OFFLINE)
			p->dead();
		delete p;
	}
	return KEEPALIVE_TIMEOUT;
}

void UdpSession::broadcastMessages()
{
	if (broadMsgList.isEmpty())
		return;

	BROADCAST_MSG *msg = LIST_ENTRY(broadMsgList.getHead(), BROADCAST_MSG, listItem);
	for (int i = 0; i < 10; ++i) {
		UdpSession *s = SessionHash::getAlive(msg->to);
		if (s)
			sendMessage(msg->type, msg->to, msg->src, s, msg->when, msg->text);
		else {
			int n = sprintf(sqlStmt, "INSERT INTO broadmsg_tbl VALUES(%lu, %lu)", msg->to, msg->id);
			mysql_real_query(&mysql, sqlStmt, n);
		}
		msg->to++;
		if (msg->to > msg->maxUIN) {
			msg->listItem.remove();
			delete msg;
			break;
		}
	}
}

void UdpSession::addFriend(uint32 dst, uint32 src, UdpSession *dstSession, UdpSession *srcSession)
{
	int n = sprintf(sqlStmt, "INSERT INTO friend_tbl values(%lu, %lu)", src, dst);
	mysql_real_query(&mysql, sqlStmt, n);

	if (!srcSession)
		return;

	if (dstSession && dstSession->status != STATUS_INVIS) {
		UdpOutPacket *out = srcSession->createPacket(UDP_SRV_USER_ONLINE);
		out->write32(dst);
		out->write32(dstSession->status);
		out->write32(dstSession->ip);
		out->write16(dstSession->port);
		out->write32(dstSession->realIP);
		srcSession->sendPacket(out);
	}
}

void UdpSession::onlineNotify()
{	
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	int n = sprintf(sqlStmt, "SELECT uin1 FROM friend_tbl WHERE uin2=%lu", uin);
	if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
		while ((row = mysql_fetch_row(res))) {
			uint32 friendUIN = atol(row[0]);
			UdpSession *session = SessionHash::getAlive(friendUIN);
			if (session) {
				UdpOutPacket *out = session->createPacket(UDP_SRV_USER_ONLINE);
				out->write32(uin);
				out->write32(status);
				out->write32(ip);
				out->write16(port);
				out->write32(realIP);
				session->sendPacket(out);
			}
		}
		mysql_free_result(res);
	}
}

void UdpSession::offlineNotify()
{		
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	int n = sprintf(sqlStmt, "SELECT uin1 FROM friend_tbl WHERE uin2=%lu", uin);
	if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
		while ((row = mysql_fetch_row(res))) {
			uint32 friendUIN = atol(row[0]);
			UdpSession *session = SessionHash::getAlive(friendUIN);
			if (session) {
				UdpOutPacket *out = session->createPacket(UDP_SRV_USER_OFFLINE);
				out->write32(uin);
				session->sendPacket(out);
			}
		}
		mysql_free_result(res);
	}
}

UdpSession::UdpSession()
{
	udpVer = 0;
	tcpVer = 0;
	sid = 0;
	uin = 0;
	auth = 0;
	ip = realIP = 0;
	port = 0;
	status = STATUS_OFFLINE;

	sendSeq = rand() & 0x7fff;
	recvSeq = 0;
	window = 0;
	expire = time(NULL) + KEEPALIVE_TIMEOUT;

	sessionCount++;
}

UdpSession::~UdpSession()
{
	IcqListItem *head = &sendQueue.head;
	IcqListItem *pos;

	while ((pos = head->next) != head) {
		UdpOutPacket *p = LIST_ENTRY(pos, UdpOutPacket, sendItem);
		pos->remove();
		p->globalSendItem.remove();
		delete p;
	}

	listItem.remove();
	keepAliveItem.remove();

	sessionCount--;
}

UdpOutPacket *UdpSession::createPacket(uint16 cmd, uint16 ackSeq)
{
	UdpOutPacket *out = new UdpOutPacket(this);
	createPacket(*out, cmd, ackSeq);
	return out;
}

void UdpSession::createPacket(UdpOutPacket &out, uint16 cmd, uint16 ackSeq)
{
	out.write16(udpVer);
	out.write32(0);
	out.write32(sid);
	out.write16(++sendSeq);
	out.write16(ackSeq);
	out.write16(cmd);
	out.write32(uin);
}

void UdpSession::sendAckPacket(uint16 cmd, uint16 seq)
{
	UdpOutPacket out(this);
	out.write16(udpVer);
	out.write32(0);
	out.write32(sid);
	out.write16(0);
	out.write16(seq);
	out.write16(cmd);
	out.write32(uin);
	sendDirect(&out);
}

void UdpSession::sendPacket(UdpOutPacket *p)
{
	p->attempts = 0;
	p->expire = time(NULL) + SEND_TIMEOUT;
	sendDirect(p);

	sendQueue.add(&p->sendItem);
	globalSendQueue.add(&p->globalSendItem);
}

bool UdpSession::setWindow(uint16 seq)
{
	if (seq >= recvSeq + 32 || seq < recvSeq)
		return false;

	if (seq == recvSeq) {
		do {
			recvSeq++;
			window >>= 1;
		} while (window & 0x1);
	} else {
		uint32 mask = (1 << (seq - recvSeq));
		if (window & mask)
			return false;
		else
			window |= mask;
	}
	return true;
}

bool UdpSession::onReceive(UdpInPacket &in)
{
	uint16 cmd = in.getCmd();
	uint16 seq = in.getSeq();

	if (cmd != UDP_LOGIN && cmd != UDP_NEW_UIN) {
		if (sid != in.getSID() || uin != in.getUIN()) {
			cout << "packet does not belong to this session." << endl;
			return false;
		}
		in.decrypt(passwd);

	} else if (sid == 0) {
		udpVer = in.getVersion();
		ip = in.getIP();
		port = in.getPort();
		recvSeq = seq;
		sid = in.getSID();
	}

	if (cmd != UDP_ACK && !setWindow(seq)) {
		cout << "packet " << seq << " is duplicated" << endl;
		sendAckPacket(cmd, seq);
		return false;
	}

	switch (cmd) {
	case UDP_ACK:
		onAck(seq);
		return true;

	case UDP_KEEPALIVE:
		onKeepAlive(in);
		break;

	case UDP_NEW_UIN:
		onNewUIN(in);
		break;

	case UDP_GET_CONTACTLIST:
		onGetContactList(in);
		break;

	case UDP_LOGIN:
		onLogin(in);
		break;

	case UDP_LOGOUT:
		onLogout(in);
		return true;

	case UDP_CHANGE_STATUS:
		sendAckPacket(cmd, seq);
		onChangeStatus(in);
		break;

	case UDP_UPDATE_CONTACT:
		onUpdateContact(in);
		break;

	case UDP_UPDATE_USER:
		onUpdateUser(in);
		break;

	case UDP_MODIFY_USER:
		sendAckPacket(cmd, seq);
		onModifyUser(in);
		break;

	case UDP_SEND_MSG:
		sendAckPacket(cmd, seq);
		onSendMessage(in);
		break;

	case UDP_GROUP_SEND_MSG:
		sendAckPacket(cmd, seq);
		//onGroupSendMessage(in);
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
		sendAckPacket(cmd, seq);
		onDelFriend(in);
		break;

	case UDP_BROADCAST_MSG:
		sendAckPacket(cmd, seq);
		onBroadcastMsg(in);
		break;

	default:
		cerr << "unknown cmd " << cmd << endl;
		return false;
	}
	return true;
}

void UdpSession::onAck(uint16 seq)
{
	IcqListItem *head = &sendQueue.head;
	IcqListItem *pos;
	UdpOutPacket *p;

	LIST_FOR_EACH(pos, head) {
		p = LIST_ENTRY(pos, UdpOutPacket, sendItem);
		if (p->getSeq() == seq) {
			cout << "packet " << seq << " is ACKed" << endl;
			pos->remove();
			p->globalSendItem.remove();
			delete p;
			if (status == STATUS_OFFLINE && sendQueue.isEmpty()) {
				cout << "session is dead!" << endl;
				delete this;
			}
			return;
		}
	}
	cout << "ACK packet(seq = " << seq << ") is ignored" << endl;
}

void UdpSession::onNewUIN(UdpInPacket &in)
{
	const char *pass = in.readString();

	int n = sprintf(sqlStmt, "INSERT INTO basic_tbl (passwd) VALUES(password('%s'))", pass);
	if (mysql_real_query(&mysql, sqlStmt, n) == 0) {
		uin = (uint32) mysql_insert_id(&mysql);
		n = sprintf(sqlStmt, "INSERT INTO ext_tbl (uin) VALUES(%lu)", uin);
		mysql_real_query(&mysql, sqlStmt, n);
	}

	UdpOutPacket *out = createPacket(UDP_NEW_UIN, in.getSeq());
	out->write32(uin);
	sendPacket(out);

	keepAliveList.add(&keepAliveItem);
	SessionHash::addDead(this);
	cout << uin << " has registered." << endl;
}

void UdpSession::onGetContactList(UdpInPacket &in)
{
	MYSQL_RES *res;
	MYSQL_ROW row;

	int n = sprintf(sqlStmt, "SELECT uin2 FROM friend_tbl WHERE uin1=%lu", uin);
	if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
		uint16 num = 0;
		UdpOutPacket *out = createPacket(UDP_GET_CONTACTLIST, in.getSeq());
		char *old = out->skip(sizeof(num));

		while ((row = mysql_fetch_row(res))) {
			out->write32(atol(row[0]));
			num++;
		}
		mysql_free_result(res);

		old = out->setCursor(old);
		out->write16(num);
		out->setCursor(old);
		sendPacket(out);
	}
}

void UdpSession::onLogin(UdpInPacket &in)
{
	uint8 error = LOGIN_INVALID_UIN;
	UdpOutPacket *out;
	uint32 friendUIN;
	MYSQL_RES *res;
	MYSQL_ROW row;

	uin = in.getUIN();
	const char *pass = in.readString();
	status = in.read32();
	tcpVer = in.read16();
	realIP = in.read32();

	int n = sprintf(sqlStmt, "SELECT auth FROM basic_tbl WHERE uin=%lu AND passwd=password('%s')", uin, pass);
	if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
		row = mysql_fetch_row(res);
		if (row) {
			auth = atoi(row[0]);
			error = LOGIN_SUCCESS;
		}
		mysql_free_result(res);
	}

	out = createPacket(UDP_LOGIN, in.getSeq());
	out->write8(error);
	sendPacket(out);

	keepAliveList.add(&keepAliveItem);

	if (error != LOGIN_SUCCESS) {
		cout << uin << " login failed." << endl;
		SessionHash::addDead(this);
		return;
	}

	strncpy(passwd, pass, 8);
	cout << uin << " successfully logged in." << endl;

	UdpSession *session = SessionHash::getAlive(uin);
	if (session) {
		cout << "previous session exists, delete it." << endl;
		delete session;
		session = NULL;
	}
	SessionHash::addAlive(this);

	// inform me of my friends' status.
	n = sprintf(sqlStmt, "SELECT uin2 FROM friend_tbl WHERE uin1=%lu", uin);
	if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
		uint16 num = 0;
		out = createPacket(UDP_SRV_MULTI_ONLINE);
		char *old = out->skip(sizeof(num));

		while ((row = mysql_fetch_row(res))) {
			friendUIN = atol(row[0]);
			UdpSession *session = SessionHash::getAlive(friendUIN);
			if (session && session->status != STATUS_INVIS) {
				num++;
				out->write32(friendUIN);
				out->write32(session->status);
				out->write32(session->ip);
				out->write16(session->port);
				out->write32(session->realIP);
				cout << "friend " << friendUIN << " is online" << endl;
			}
		}
		if (num) {
			old = out->setCursor(old);
			out->write16(num);
			out->setCursor(old);
			sendPacket(out);
		} else {
			sendSeq--;
			delete out;
		}

		mysql_free_result(res);
	}

	// I am now online!
	if (status == STATUS_OFFLINE)
		status = STATUS_ONLINE;
	if (status != STATUS_INVIS)
		onlineNotify();

	// Broadcast messages
	n = sprintf(sqlStmt, "SELECT src, type, time, msg FROM broadmsg_tbl a, broadmsg_content_tbl b "
		"WHERE dst=%lu AND a.id=b.id", uin);
	sendMessages(sqlStmt, n);
	
	// Has someone sent me messages during offline?
	n = sprintf(sqlStmt, "SELECT src, type, time, msg FROM message_tbl WHERE dst=%lu", uin);
	sendMessages(sqlStmt, n);
}

void UdpSession::sendMessages(const char *sql, int n)
{
	MYSQL_RES *res;
	MYSQL_ROW row;

	if (mysql_real_query(&mysql, sql, n) == 0 && (res = mysql_store_result(&mysql))) {
		while ((row = mysql_fetch_row(res))) {
			uint32 friendUIN = atol(row[0]);
			uint8 type = atoi(row[1]);
			time_t sendTime = atol(row[2]);
			const char *text = row[3];

			UdpOutPacket *out = createPacket(UDP_SRV_MESSAGE);
			out->write8(type);
			out->write32(friendUIN);
			out->write32(sendTime);
			out->writeString(text);
			sendPacket(out);
		}
		mysql_free_result(res);
	}
}

void UdpSession::sendMessage(uint8 type, uint32 dst, uint32 src, UdpSession *dstSession, time_t when, const char *text)
{
	if (dstSession) {
		UdpOutPacket *out = dstSession->createPacket(UDP_SRV_MESSAGE);
		out->write8(type);
		out->write32(src);
		out->write32(when);
		out->writeString(text);
		dstSession->sendPacket(out);
	} else {
		int n = sprintf(sqlStmt, "INSERT INTO message_tbl values(%lu, %lu, %d, %lu, '%s')",
			dst, src, (int) type, when, text);
		mysql_real_query(&mysql, sqlStmt, n);
	}
}

void UdpSession::onUpdateContact(UdpInPacket &in)
{
	uint32 dst = in.read32();
	MYSQL_RES *res;
	MYSQL_ROW row;

	int n = sprintf(sqlStmt, "SELECT pic, nick, age, gender, country, province, city, email,"
		"address, zipcode, tel, name, blood, college, profession, homepage, intro "
		"FROM basic_tbl a, ext_tbl b WHERE a.uin=%lu AND a.uin=b.uin", dst);
	if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
		row = mysql_fetch_row(res);
		if (row) {
			UdpOutPacket *out = createPacket(UDP_UPDATE_CONTACT, in.getSeq());
			out->write32(dst);

			out->write8(atoi(row[0]));
			out->writeString(row[1]);
			out->write8(atoi(row[2]));
			out->write8(atoi(row[3]));

			out->writeString(row[4]);
			out->writeString(row[5]);
			out->writeString(row[6]);

			out->writeString(row[7]);
			out->writeString(row[8]);
			out->writeString(row[9]);
			out->writeString(row[10]);

			out->writeString(row[11]);
			out->write8(atoi(row[12]));
			out->writeString(row[13]);
			out->writeString(row[14]);
			out->writeString(row[15]);
			out->writeString(row[16]);

			sendPacket(out);
		}
		mysql_free_result(res);
	}
}

void UdpSession::onUpdateUser(UdpInPacket &in)
{
	MYSQL_RES *res;
	MYSQL_ROW row;

	int n = sprintf(sqlStmt, "SELECT pic, nick, age, gender, country, province, city, email, "
		"address, zipcode, tel, name, blood, college, profession, homepage, intro "
		"FROM basic_tbl a, ext_tbl b WHERE a.uin=%lu AND a.uin=b.uin", uin);
	if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
		row = mysql_fetch_row(res);
		if (row) {
			UdpOutPacket *out = createPacket(UDP_UPDATE_USER, in.getSeq());

			out->write8(atoi(row[0]));
			out->writeString(row[1]);
			out->write8(atoi(row[2]));
			out->write8(atoi(row[3]));
			out->writeString(row[4]);
			out->writeString(row[5]);
			out->writeString(row[6]);

			out->writeString(row[7]);
			out->writeString(row[8]);
			out->writeString(row[9]);
			out->writeString(row[10]);

			out->writeString(row[11]);
			out->write8(atoi(row[12]));
			out->writeString(row[13]);
			out->writeString(row[14]);
			out->writeString(row[15]);
			out->writeString(row[16]);

			out->write8(auth);

			sendPacket(out);
		}
		mysql_free_result(res);
	}
}

void UdpSession::onModifyUser(UdpInPacket &in)
{
	int pic = in.read8();
	const char *nick = in.readString();
	int age = in.read8();
	int gender = in.read8();
	const char *country = in.readString();
	const char *province = in.readString();
	const char *city = in.readString();
	const char *email = in.readString();
	const char *address = in.readString();
	const char *zipcode = in.readString();
	const char *tel = in.readString();
	const char *name = in.readString();
	int blood = in.read8();
	const char *college = in.readString();
	const char *profession = in.readString();
	const char *homepage = in.readString();
	const char *intro = in.readString();
	auth = in.read8();
	uint8 modifyPasswd = in.read8();

	char *p = sqlStmt;
	p += sprintf(p, "UPDATE basic_tbl SET ");
	if (modifyPasswd) {
		const char *pass = in.readString();
		if (*pass)
			p += sprintf(p, "passwd=password('%s'), ", pass);
	}
	p += sprintf(p, "nick='%s', pic=%d, gender=%d, age=%d, country='%s', province='%s', city='%s', email='%s', auth=%d "
		"WHERE uin=%lu", nick, pic, gender, age, country, province, city, email, auth, uin);
	int n = p - sqlStmt;
	mysql_real_query(&mysql, sqlStmt, n);

	n = sprintf(sqlStmt, "UPDATE ext_tbl SET "
		"address='%s', zipcode='%s', tel='%s', "
		"name='%s', blood=%d, college='%s', profession='%s', homepage='%s', intro='%s' WHERE uin=%lu",
		address, zipcode, tel, name, blood, college, profession, homepage, intro, uin);
	mysql_real_query(&mysql, sqlStmt, n);
}

void UdpSession::dead()
{
	if (status != STATUS_INVIS)
		offlineNotify();
	status = STATUS_OFFLINE;
}

void UdpSession::onLogout(UdpInPacket &in)
{
	cout << uin << " logged out." << endl;
	dead();

	if (sendQueue.isEmpty()) {
		// delete all offline messages
		int n = sprintf(sqlStmt, "DELETE FROM message_tbl WHERE dst=%lu", uin);
		mysql_real_query(&mysql, sqlStmt, n);

		n = sprintf(sqlStmt, "DELETE FROM broadmsg_tbl WHERE dst=%lu", uin);
		mysql_real_query(&mysql, sqlStmt, n);
	}
	delete this;
}

void UdpSession::onChangeStatus(UdpInPacket &in)
{
	uint32 oldStatus = status;
	status = in.read32();

	if (oldStatus == STATUS_INVIS && status != STATUS_INVIS)
		onlineNotify();
	else if (oldStatus != STATUS_INVIS && status == STATUS_INVIS)
		offlineNotify();
	else if (status != STATUS_INVIS) {
		MYSQL_RES *res;
		MYSQL_ROW row;

		int n = sprintf(sqlStmt, "SELECT uin1 FROM friend_tbl WHERE uin2=%lu", uin);
		if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
			while ((row = mysql_fetch_row(res))) {
				uint32 friendUIN = atol(row[0]);
				UdpSession *session = SessionHash::getAlive(friendUIN);
				if (session) {
					UdpOutPacket *out = session->createPacket(UDP_SRV_STATUS_CHANGED);
					out->write32(uin);
					out->write32(status);
					session->sendPacket(out);
				}
			}
			mysql_free_result(res);
		}
	}
}

void UdpSession::onKeepAlive(UdpInPacket &in)
{
	cout << uin << " keeps alive" << endl;
	
	expire = time(NULL) + KEEPALIVE_TIMEOUT;
	keepAliveItem.remove();
	keepAliveList.add(&keepAliveItem);

	UdpOutPacket out(this);
	createPacket(out, UDP_KEEPALIVE, in.getSeq());
	out.write32(sessionCount);
	sendDirect(&out);
}

void UdpSession::onSendMessage(UdpInPacket &in)
{
	uint8 type = in.read8();
	uint32 to = in.read32();
	time_t when = in.read32();
	const char *text = in.readString();

	UdpSession *session = SessionHash::getAlive(to);
	sendMessage(type, to, uin, session, when, text);

	if (type == MSG_AUTH_ACCEPTED)
		addFriend(uin, to, this, session);
}

void UdpSession::onBroadcastMsg(UdpInPacket &in)
{
	if (uin >= START_UIN)
		return;
	
	uint8 type = in.read8();
	uint32 when = in.read32();
	time_t expire = in.read32();
	const char *text = in.readString();

	if (expire <= time(NULL) || !*text)
		return;

	int n = sprintf(sqlStmt, "INSERT INTO broadmsg_content_tbl "
		"VALUES(NULL, %lu, %d, %lu, %lu, '%s')", uin, type, when, expire, text);
	mysql_real_query(&mysql, sqlStmt, n);

	BROADCAST_MSG *msg = new BROADCAST_MSG;
	msg->id = (uint32) mysql_insert_id(&mysql);
	msg->type = type;
	msg->src = uin;
	msg->when = when;
	strncpy(msg->text, text, MAX_MSG_LEN);
	msg->expire = expire;
	msg->to = START_UIN;

	MYSQL_RES *res;
	MYSQL_ROW row;
	static const char sql[] = "SELECT MAX(uin) FROM basic_tbl";
	if (mysql_real_query(&mysql, sql, sizeof(sql) - 1) == 0 && (res = mysql_store_result(&mysql))) {
		row = mysql_fetch_row(res);
		if (row)
			msg->maxUIN = atoi(row[0]);
		mysql_free_result(res);
	}

	broadMsgList.add(&msg->listItem);
}

void UdpSession::onGroupSendMessage(UdpInPacket &in)
{
	uint8 type = in.read8();
	time_t when = in.read32();
	const char *text = in.readString();

	int n = in.read16();
	while (n-- > 0) {
		uint32 to = in.read32();
		UdpSession *session = SessionHash::getAlive(to);
		sendMessage(type, to, uin, session, when, text);
	}
}

void UdpSession::onSearchRandom(UdpInPacket &in)
{
	uint32 results[MAX_SEARCH_PER_PAGE];
	uint16 n = SessionHash::random(results, MAX_SEARCH_PER_PAGE);
	UdpOutPacket *out = createPacket(UDP_SRV_SEARCH, in.getSeq());
	out->write16(n);

	if (n > 0) {
		MYSQL_RES *res;
		MYSQL_ROW row;

		static const char sql[] = "SELECT uin, pic, nick, province FROM basic_tbl WHERE uin IN(";
		memcpy(sqlStmt, sql, sizeof(sql));
		char *p = sqlStmt + sizeof(sql) - 1;
		for (int i = 0; i < n; i++)
			p += sprintf(p, "%lu,", results[i]);
		*(p - 1) = ')';

		if (mysql_real_query(&mysql, sqlStmt, p - sqlStmt) == 0 &&
			(res = mysql_store_result(&mysql))) {
			while ((row = mysql_fetch_row(res))) {
				uint32 contactUIN = atoi(row[0]);
				uint8 pic = atoi(row[1]);
				const char *nick = row[2];
				const char *province = row[3];

				out->write32(contactUIN);
				out->write8(1);		// online
				out->write8(pic);
				out->writeString(nick);
				out->writeString(province);
			}
			mysql_free_result(res);
		}
	}

	sendPacket(out);
}

void UdpSession::onSearchCustom(UdpInPacket &in)
{
	uint32 uin = in.read32();
	const char *nick = in.readString();
	const char *email = in.readString();
	uint32 startUIN = in.read32();
	int n;

	if (uin != 0) {
		n = sprintf(sqlStmt, "SELECT uin, pic, nick, province FROM basic_tbl WHERE uin=%lu LIMIT %d",
			uin, MAX_SEARCH_PER_PAGE);
	} else if (*nick || *email) {
		char *p = sqlStmt;
		p += sprintf(p, "SELECT uin, pic, nick, province FROM basic_tbl WHERE uin>%lu AND ", startUIN);

		if (*nick)
			p += sprintf(p, "nick='%s'", nick);
		if (*email) {
			if (*nick)
				p += sprintf(p, " AND ");
			p += sprintf(p, "email='%s'", email);
		}
		p += sprintf(p, " LIMIT %d", MAX_SEARCH_PER_PAGE);
		n = p - sqlStmt;
	} else {
		cout << "unknown search mode" << endl;
		return;
	}

	MYSQL_RES *res;
	MYSQL_ROW row;
	UdpOutPacket *out;

	if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {

		uint16 num = 0;
		out = createPacket(UDP_SRV_SEARCH, in.getSeq());
		char *old = out->skip(sizeof(num));

		while ((row = mysql_fetch_row(res))) {
			uint32 contactUIN = atoi(row[0]);
			uint8 pic = atoi(row[1]);
			const char *nick = row[2];
			const char *province = row[3];

			out->write32(contactUIN);
			out->write8(SessionHash::getAlive(contactUIN) ? 1 : 0);
			out->write8(pic);
			out->writeString(nick);
			out->writeString(province);

			num++;
		}
		mysql_free_result(res);

		old = out->setCursor(old);
		out->write16(num);
		out->setCursor(old);
		sendPacket(out);
	}
}

void UdpSession::onAddFriend(UdpInPacket &in)
{
	uint32 dst = in.read32();
	uint8 dstAuth = 0;

	UdpSession *dstSession = SessionHash::getAlive(dst);
	if (dstSession)
		dstAuth = dstSession->auth;
	else {
		MYSQL_RES *res;
		MYSQL_ROW row;
		int n = sprintf(sqlStmt, "SELECT auth FROM basic_tbl WHERE uin=%lu", dst);
		if (mysql_real_query(&mysql, sqlStmt, n) == 0 && (res = mysql_store_result(&mysql))) {
			row = mysql_fetch_row(res);
			if (row)
				dstAuth = atoi(row[0]);
			mysql_free_result(res);
		}
	}

	UdpOutPacket *out = createPacket(UDP_ADD_FRIEND, in.getSeq());
	out->write32(dst);
	out->write8(dstAuth);
	sendPacket(out);

	if (dstAuth == ADD_FRIEND_ACCEPTED) {
		addFriend(dst, uin, dstSession, this);
		sendMessage(MSG_ADDED, dst, uin, dstSession, time(NULL), "");
	}
}

void UdpSession::onDelFriend(UdpInPacket &in)
{
	uint32 dst = in.read32();

	int n = sprintf(sqlStmt, "DELETE FROM friend_tbl WHERE uin1=%lu AND uin2=%lu", uin, dst);
	mysql_real_query(&mysql, sqlStmt, n);
}
