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

#include "server.h"
#include "myicq.h"
#include "log.h"
#include "utils.h"
#include "udpsession.h"
#include "sessionhash.h"
#include "dnsmanager.h"
#include <string.h>

#define MYICQ_S2S_PORT		8222
#define SERVER_HASH_SIZE	128
#define S2S_PROTO_VER		1

#define CONNECT_TIMEOUT		30
#define ESTABLISH_TIMEOUT	40

enum {
	TCP_STATUS_NOT_CONN,
	TCP_STATUS_CONNECTING,
	TCP_STATUS_CONNECTED,
	TCP_STATUS_HELLO_WAIT,
	TCP_STATUS_AUTH_WAIT,
	TCP_STATUS_ESTABLISHED,
};

enum {
	TCP_CMD_HELLO = 0x4000,
	TCP_CMD_USER_ONLINE,
	TCP_CMD_USER_OFFLINE,
	TCP_CMD_USER_STATUS,
	TCP_CMD_ADD_FRIEND,
	TCP_CMD_DEL_FRIEND,
	TCP_CMD_SEND_MESSAGE,
	TCP_CMD_UPDATE_CONTACT,
	TCP_CMD_SEARCH_RANDOM,
	TCP_CMD_SEARCH_UIN,

	TCP_CMD_HELLO_REPLY = 0x5000,
	TCP_CMD_ADD_FRIEND_REPLY,
	TCP_CMD_UPDATE_CONTACT_REPLY,
	TCP_CMD_SEARCH_RANDOM_REPLY,
	TCP_CMD_SEARCH_UIN_REPLY,
};


class ServerHash {
public:
	void put(Server *s) {
		int i = hashfn(s->ip);

		// Note that we add it to the head of the bucket list,
		// this is really a trick:-)
		hash[i].addHead(&s->hashItem);
	}
	Server *get(uint32 ip);

private:
	int hashfn(uint32 ip) {
		return (ip & (SERVER_HASH_SIZE - 1));
	}

	ListHead hash[SERVER_HASH_SIZE];
};

Server *ServerHash::get(uint32 ip)
{
	int i = hashfn(ip);
	ListHead *head = &hash[i];
	ListHead *pos;

	LIST_FOR_EACH(pos, head) {
		Server *s = LIST_ENTRY(pos, Server, hashItem);
		if (s->ip == ip)
			return s;
	}
	return NULL;
}


ListHead Server::serverList;
static ServerHash serverHash;
static int listenSock = -1;
static ICQ_STR emptyStr = { "", 0 };

IMPLEMENT_SLAB(RemoteSession, 128)
IMPLEMENT_SLAB(Server, 16)


static Server *acceptServer()
{
	sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int sock = accept(listenSock, (sockaddr *) &addr, &addrlen);
	if (sock < 0)
		return NULL;

	uint32 ip = addr.sin_addr.s_addr;

	Server *s = serverHash.get(ip);
	if (s && s->isAccepted) {
		LOG(3) ("Server %s was already accepted.\n", myicq_inet_ntoa(ip));
		close(sock);
		return NULL;
	}

	unsigned long on = 1;
	if (ioctl(sock, FIONBIO, &on) < 0) {
		close(sock);
		return NULL;
	}

	s = new Server;
	s->ip = ip;
	s->sock = sock;
	s->status = TCP_STATUS_HELLO_WAIT;
	s->expire = curTime + ESTABLISH_TIMEOUT;
	s->isAccepted = true;

	serverHash.put(s);
	return s;
}

static void onlineNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	Server *server = (Server *) req->refObj;
	uint32 uin = (uint32) req->data;
	Session *s = server->getSession(uin);
	if (!s)
		return;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		UdpSession *session = SessionHash::get(atol(row[0]));
		if (session)
			session->sendOnline(s, server->domain);
	}
}

static void offlineNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	Server *server = (Server *) req->refObj;
	uint32 uin = (uint32) req->data;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		UdpSession *session = SessionHash::get(atol(row[0]));
		if (session)
			session->sendOffline(uin, server->domain);
	}
}

static void statusNotifyCB(DBRequest *req)
{
	if (!req->res)
		return;

	Server *server = (Server *) req->refObj;
	uint32 uin = (uint32) req->data;
	Session *s = server->getSession(uin);
	if (!s)
		return;

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(req->res))) {
		UdpSession *session = SessionHash::get(atol(row[0]));
		if (session)
			session->sendStatusChanged(s, server->domain);
	}
}

static Server *getServer(const char *name, uint32 ip)
{
	if (!name || !*name)
		return NULL;

	Server *s = NULL;

	if (ip == INADDR_NONE) {
		IPEntry *e = IPCache::get(name);
		if (e) {
			ip = e->ip;
			s = e->server;
		}
	}
	if (!s && ip != INADDR_NONE)
		s = serverHash.get(ip);

	return s;
}

Server *Server::getServer(const char *name)
{
	uint32 ip = inet_addr(name);
	return ::getServer(name, ip);
}

Server *Server::createServer(const char *name)
{
	uint32 ip = inet_addr(name);

	Server *s = ::getServer(name, ip);
	if (!s) {
		s = new Server;

		if (ip == INADDR_NONE)
			DNSManager::resolve(name, s);
		else
			s->onDnsResolved(ip);
	}
	return s;
}

RemoteSession *Server::getSession(QID &qid)
{
	Server *s = getServer(qid.domain.text);
	return (s ? s->getSession(qid.uin) : NULL);
}

RemoteSession *Server::createSession(uint32 uin)
{
	RemoteSession *s = getSession(uin);
	if (!s) {
		s = new RemoteSession(uin);

		int i = (uin & (UIN_HASH_SIZE - 1));
		uinHash[i].add(&s->hashItem);
		sessionList.add(&s->listItem);
	}
	return s;
}

int Server::generateFds(fd_set &readfds, fd_set &writefds)
{
	int maxfd = listenSock;

	FD_SET(listenSock, &readfds);

	ListHead *head = &serverList, *pos;
	LIST_FOR_EACH(pos, head) {
		Server *s = LIST_ENTRY(pos, Server, listItem);
		int sock = s->sock;
		if (sock < 0)
			continue;

		if (sock > maxfd)
			maxfd = sock;

		FD_SET(sock, &readfds);
		if (s->status == TCP_STATUS_CONNECTING)
			FD_SET(sock, &writefds);
	}

	return maxfd;
}

void Server::examineFds(fd_set &readfds, fd_set &writefds)
{
	ListHead *head = &serverList;
	ListHead *pos, *next;

	for (pos = head->next; pos != head; pos = next) {
		next = pos->next;

		Server *s = LIST_ENTRY(pos, Server, listItem);
		int sock = s->sock;
		if (sock < 0)
			continue;

		if (s->expire && s->expire <= curTime) {
			LOG(3) ("Server %s timeouts.\n", myicq_inet_ntoa(s->ip));
			s->onClose();
			continue;
		}

		if (FD_ISSET(sock, &readfds))
			s->onReceive();
		if (s->status == TCP_STATUS_CONNECTING) {
			if (FD_ISSET(sock, &writefds))
				s->onConnect();
		}
	}

	if (FD_ISSET(listenSock, &readfds))
		acceptServer();
}

bool Server::init()
{
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock < 0)
		return false;

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(MYICQ_S2S_PORT);
	if (bind(listenSock, (sockaddr *) &addr, sizeof(addr)) < 0) {
		close(listenSock);
		return false;
	}

	if (listen(listenSock, 5) < 0) {
		close(listenSock);
		return false;
	}

	return true;
}

void Server::destroy()
{
	if (listenSock >= 0)
		close(listenSock);
}

Server::Server()
{
	*domainName = '\0';
	domain.text = domainName;
	domain.len = 0;

	*descName = '\0';
	desc.text = descName;
	desc.len = 0;

	ip = INADDR_NONE;

	sock = -1;
	status = TCP_STATUS_NOT_CONN;
	expire = 0;
	sessionCount = 0;

	isAccepted = false;
	bufSize = 0;

	serverList.add(&listItem);
}

Server::~Server()
{
	ListHead *head = &sendQueue, *pos;
	while ((pos = head->next) != head) {
		pos->remove();
		TcpOutPacket *p = LIST_ENTRY(pos, TcpOutPacket, listItem);
		delete p;
	}

	head = &sessionList;
	while ((pos = head->next) != head) {
		pos->remove();
		RemoteSession *s = LIST_ENTRY(pos, RemoteSession, listItem);
		delete s;
	}

	if (sock >= 0)
		close(sock);
}

bool Server::setDomainAndDesc(ICQ_STR &name, ICQ_STR &des)
{
	int n = name.len;
	if (!n || n > MAX_DOMAIN_NAME)
		return false;

	strLowerCase(name.text);

#ifndef _DEBUG
	if (_ops.domain == name.text) {
		LOG(4) ("Cannot connect to the local server.\n");
		return false;
	}
#endif

	memcpy(domainName, name.text, n + 1);
	domain.len = name.len;

	n = des.len;
	if (n > MAX_DESC_NAME)
		n = MAX_DESC_NAME;
	memcpy(descName, des.text, n + 1);
	descName[n] = '\0';
	desc.len = n;

	return true;
}

void Server::notify(uint32 uin, DB_CALLBACK cb)
{
	DBRequest *req = new DBRequest(false, cb, this, uin);
	WRITE_STR(req, "SELECT uin1 FROM remote_friend_tbl WHERE uin2=");
	*req << uin;
	WRITE_STR(req, " AND domain=");
	*req << domain;
	WRITE_STR(req, " LIMIT 128");

	DBManager::query(req);
}

void Server::createPacket(TcpOutPacket &out, uint16 cmd)
{
	out << (uint16) S2S_PROTO_VER << (uint32) 0;
	out << cmd << UdpSession::sessionCount;
}

TcpOutPacket *Server::createPacket(uint16 cmd)
{
	TcpOutPacket *p;

	if (status != TCP_STATUS_ESTABLISHED)
		p = new TcpOutPacket;
	else {
		static TcpOutPacket out;
		p = &out;
		p->reset();
	}

	createPacket(*p, cmd);
	return p;
}

void Server::sendPacket(TcpOutPacket *p)
{
	if (status != TCP_STATUS_ESTABLISHED)
		sendQueue.add(&p->listItem);
	else
		p->send(sock);
}

void Server::sendPendingPacket(int s)
{
	ListHead *head = &sendQueue, *pos;

	while ((pos = head->next) != head) {
		pos->remove();
		TcpOutPacket *p = LIST_ENTRY(pos, TcpOutPacket, listItem);
		p->send(s);
		delete p;
	}
}

RemoteSession *Server::getSession(uint32 uin)
{
	int i = (uin & (UIN_HASH_SIZE - 1));
	ListHead *head = &uinHash[i], *pos;

	LIST_FOR_EACH(pos, head) {
		RemoteSession *s = LIST_ENTRY(pos, RemoteSession, hashItem);
		if (s->uin == uin)
			return s;
	}
	return NULL;
}

bool Server::connect(uint32 ip, uint16 port)
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		LOG(1) ("socket() failed\n");
		return false;
	}

	unsigned long on = 1;
	if (ioctl(sock, FIONBIO, &on) < 0) {
		LOG(1) ("ioctl() failed\n");
		return false;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

	if (::connect(sock, (sockaddr *) &addr, sizeof(addr)) == 0)
		onConnect();
	else if (getSocketError() == EINPROGRESS) {
		expire = curTime + CONNECT_TIMEOUT;
		status = TCP_STATUS_CONNECTING;
	} else {
		LOG(2) ("Cannot connect to %s failed\n", myicq_inet_ntoa(ip));
		return false;
	}

	return true;
}

void Server::onDnsResolved(uint32 ip)
{
	if (ip == INADDR_NONE) {
		LOG(3) ("Can not resolve host\n");
		onClose();
		return;
	}

	if (status == TCP_STATUS_NOT_CONN) {
		Server *s = serverHash.get(ip);
		if (s) {
			LOG(4) ("Server %s already exists.\n", myicq_inet_ntoa(ip));

			sendPendingPacket(s->sock);
			onClose();
			return;
		}
		if (connect(ip, MYICQ_S2S_PORT)) {
			this->ip = ip;
			serverHash.put(this);
		}
	} else if (status == TCP_STATUS_AUTH_WAIT) {
		if (this->ip != ip) {
			LOG(2) ("s2s authentication failed\n");
			onClose();
			return;
		}

		LOG(1) ("%s is accepted\n", myicq_inet_ntoa(ip));

		status = TCP_STATUS_ESTABLISHED;
		expire = 0;

		TcpOutPacket *out = createPacket(TCP_CMD_HELLO_REPLY);
		*out << _ops.domain.c_str();
		*out << _ops.desc.c_str();
		sendPacket(out);
	}
}

void Server::onConnect()
{
	LOG(4) ("Connected to %s.\n", myicq_inet_ntoa(ip));
	
	status = TCP_STATUS_CONNECTED;
	expire = curTime + ESTABLISH_TIMEOUT;

	TcpOutPacket out;
	createPacket(out, TCP_CMD_HELLO);
	out << _ops.domain.c_str();
	out << _ops.desc.c_str();
	out.send(sock);
}

void Server::onReceive()
{
	int n;
	while ((n = recv(sock, buf + bufSize, TCP_PACKET_SIZE - bufSize, 0)) > 0) {
		bufSize += n;
		char *start = buf;
		char *end = start + bufSize;
		uint16 len;

		while (start + sizeof(len) < end) {
			len = ntohs(*(uint16 *) start);
			if (end - start - sizeof(len) < len)
				break;

			start += sizeof(len);
			if (len >= sizeof(TCP_HEADER)) {
				TcpInPacket in(start, len);
				onPacketReceived(in);
			}
			start += len;
		}

		bufSize = end - start;
		if (bufSize > 0)
			memcpy(buf, start, bufSize);
	}
	if (n == 0) {
		LOG(2) ("%s connection lost\n", myicq_inet_ntoa(ip));
		onClose();
	}
}

void Server::onClose()
{
	hashItem.remove();
	listItem.remove();
	release();
}

void Server::sendMessage(uint8 type, uint32 dst, uint32 src, ICQ_STR &text)
{
	TcpOutPacket *out = createPacket(TCP_CMD_SEND_MESSAGE);
	*out << type << dst << src << text;
	sendPacket(out);
}

void Server::updateContact(uint16 seq, uint32 dst, uint32 src)
{
	TcpOutPacket *out = createPacket(TCP_CMD_UPDATE_CONTACT);
	*out << seq << dst << src;
	sendPacket(out);
}

void Server::searchRandom(uint16 seq, uint32 src)
{
	TcpOutPacket *out = createPacket(TCP_CMD_SEARCH_RANDOM);
	*out << seq << src;
	sendPacket(out);
}

void Server::searchUIN(uint16 seq, uint32 dst, uint32 src)
{
	TcpOutPacket *out = createPacket(TCP_CMD_SEARCH_UIN);
	*out << seq << dst << src;
	sendPacket(out);
}

void Server::addFriendReply(uint16 seq, uint32 dst, uint32 src, uint8 auth)
{
	TcpOutPacket *out = createPacket(TCP_CMD_ADD_FRIEND_REPLY);
	*out << seq << dst << src << auth;
	sendPacket(out);

	RemoteSession *s = Server::getSession(dst);
	if (s)
		sendUserOnline(s);
}

void Server::updateContactReply(uint16 seq, uint32 src, uint8 *data, int n)
{
	TcpOutPacket *out = createPacket(TCP_CMD_UPDATE_CONTACT_REPLY);
	*out << seq << src;
	out->writeData(data, n);
	sendPacket(out);
}

void Server::searchUINReply(uint16 seq, uint32 src, uint8 *data, int n)
{
	TcpOutPacket *out = createPacket(TCP_CMD_SEARCH_UIN_REPLY);
	*out << seq << src;
	out->writeData(data, n);
	sendPacket(out);
}

void Server::addFriend(uint16 seq, uint32 dst, uint32 src)
{
	TcpOutPacket *out = createPacket(TCP_CMD_ADD_FRIEND);
	*out << seq << dst << src;
	sendPacket(out);
}

void Server::delFriend(uint32 dst, uint32 src)
{
	TcpOutPacket *out = createPacket(TCP_CMD_DEL_FRIEND);
	*out << dst << src;
	sendPacket(out);
}

void Server::sendUserOnline(Session *s)
{
	TcpOutPacket *out = createPacket(TCP_CMD_USER_ONLINE);

	*out << s->uin << s->status;
	out->write32(s->ip);
	out->write16(s->msgport);
	out->write32(s->realip);

	sendPacket(out);
}

void Server::sendUserOffline(uint32 uin)
{
	TcpOutPacket *out = createPacket(TCP_CMD_USER_OFFLINE);
	*out << uin;
	sendPacket(out);
}

void Server::sendUserStatus(uint32 uin, uint32 status)
{
	TcpOutPacket *out = createPacket(TCP_CMD_USER_STATUS);
	*out << uin << status;
	sendPacket(out);
}

void Server::onHello(TcpInPacket &in)
{
	if (status != TCP_STATUS_HELLO_WAIT)
		return;

	status = TCP_STATUS_AUTH_WAIT;

	ICQ_STR name, des;
	in >> name >> des;

	if (!setDomainAndDesc(name, des)) {
		onClose();
		return;
	}

	uint32 ip = inet_addr(domainName);
	if (ip == INADDR_NONE) {
		IPEntry *e = IPCache::get(domainName);
		if (e && !e->server) {
			onDnsResolved(e->ip);
			return;
		}
	}
	DNSManager::resolve(domainName, this);
}

void Server::onUserOnline(TcpInPacket &in)
{
	uint32 uin;
	in >> uin;

	RemoteSession *s = createSession(uin);

	in >> s->status;
	s->ip = in.read32();
	s->msgport = in.read16();
	s->realip = in.read32();

	notify(uin, onlineNotifyCB);
}

void Server::onUserOffline(TcpInPacket &in)
{
	uint32 uin;
	in >> uin;

	RemoteSession *s = getSession(uin);
	if (s) {
		s->hashItem.remove();
		s->listItem.remove();
		delete s;

		notify(uin, offlineNotifyCB);
	}
}

void Server::onUserStatus(TcpInPacket &in)
{
	uint32 uin, status;
	in >> uin >> status;

	RemoteSession *s = getSession(uin);
	if (s && s->status != status) {
		s->status = status;
		notify(uin, statusNotifyCB);
	}
}

void Server::onAddFriend(TcpInPacket &in)
{
	uint16 seq;
	uint32 src;
	QID dst;

	in >> seq >> dst.uin >> src;
	dst.domain = emptyStr;

	UdpSession::addFriend(seq, dst, src, this);
}

void Server::onDelFriend(TcpInPacket &in)
{
	uint32 dst, src;
	in >> dst >> src;
	
	DBRequest *req = new DBRequest(true);
	WRITE_STR(req, "DELETE FROM notify_tbl WHERE uin1=");
	*req << dst;
	WRITE_STR(req, " AND uin2=");
	*req << src;
	WRITE_STR(req, " AND domain=");
	*req << domain;

	DBManager::query(req);
}

void Server::onSendMessage(TcpInPacket &in)
{
	uint8 type;
	QID dst, src;
	ICQ_STR text;

	in >> type >> dst.uin >> src.uin >> text;
	src.domain = domain;
	dst.domain = emptyStr;
	UdpSession::sendMessage(type, dst, src, curTime, text);
}

void Server::onUpdateContact(TcpInPacket &in)
{
	uint16 seq;
	uint32 dst, src;

	in >> seq >> dst >> src;

	UdpSession::updateContact(seq, dst, src, this);
}

void Server::onHelloReply(TcpInPacket &in)
{
	if (status != TCP_STATUS_CONNECTED)
		return;
	
	status = TCP_STATUS_ESTABLISHED;
	expire = 0;

	ICQ_STR name, des;
	in >> name >> des;
	if (!setDomainAndDesc(name, des)) {
		onClose();
		return;
	}

	sendPendingPacket(sock);
}

void Server::onAddFriendReply(TcpInPacket &in)
{
	uint16 seq;
	QID dst;
	uint32 src;
	uint8 auth;

	in >> seq >> dst.uin >> src >> auth;
	dst.domain = domain;

	UdpSession::addFriendAuth(seq, dst, src, NULL, auth, false);
}

void Server::onUpdateContactReply(TcpInPacket &in)
{
	uint16 seq;
	uint32 src;

	in >> seq >> src;

	UdpSession *s = SessionHash::get(src);
	if (!s)
		return;

	int n;
	uint8 *data = in.readData(n);
	s->updateContactReply(seq, data, n, this);
}

void Server::onSearchRandom(TcpInPacket &in)
{
	uint16 seq;
	uint32 src;

	in >> seq >> src;

	TcpOutPacket *out = createPacket(TCP_CMD_SEARCH_RANDOM_REPLY);
	*out << seq << src;
	SessionHash::random(*out, 25);
	sendPacket(out);
}

void Server::onSearchUIN(TcpInPacket &in)
{
	uint16 seq;
	uint32 dst, src;

	in >> seq >> dst >> src;

	UdpSession::searchUIN(seq, dst, src, this);
}

void Server::onSearchRandomReply(TcpInPacket &in)
{
	uint16 seq;
	uint32 src;

	in >> seq >> src;

	UdpSession *s = SessionHash::get(src);
	if (!s)
		return;

	int n;
	uint8 *data = in.readData(n);
	s->searchRandomReply(seq, data, n);
}

void Server::onSearchUINReply(TcpInPacket &in)
{
	uint16 seq;
	uint32 src;

	in >> seq >> src;

	UdpSession *s = SessionHash::get(src);
	if (!s)
		return;

	int n;
	uint8 *data = in.readData(n);
	s->searchUINReply(seq, data, n);
}

void Server::onPacketReceived(TcpInPacket &in)
{
	sessionCount = in.header.sessionCount;

	switch (in.header.cmd) {
	case TCP_CMD_HELLO:
		onHello(in);
		return;

	case TCP_CMD_HELLO_REPLY:
		onHelloReply(in);
		return;
	}

	if (status != TCP_STATUS_ESTABLISHED) {
		LOG(3) ("cmd %d can not be processed "
				"since connection has not been established\n", in.header.cmd);
		return;
	}
	
	switch (in.header.cmd) {
	case TCP_CMD_USER_ONLINE:
		onUserOnline(in);
		break;

	case TCP_CMD_USER_OFFLINE:
		onUserOffline(in);
		break;

	case TCP_CMD_USER_STATUS:
		onUserStatus(in);
		break;

	case TCP_CMD_ADD_FRIEND:
		onAddFriend(in);
		break;

	case TCP_CMD_DEL_FRIEND:
		onDelFriend(in);
		break;

	case TCP_CMD_SEND_MESSAGE:
		onSendMessage(in);
		break;

	case TCP_CMD_UPDATE_CONTACT:
		onUpdateContact(in);
		break;

	case TCP_CMD_SEARCH_RANDOM:
		onSearchRandom(in);
		break;

	case TCP_CMD_SEARCH_UIN:
		onSearchUIN(in);
		break;

	case TCP_CMD_ADD_FRIEND_REPLY:
		onAddFriendReply(in);
		break;

	case TCP_CMD_UPDATE_CONTACT_REPLY:
		onUpdateContactReply(in);
		break;

	case TCP_CMD_SEARCH_RANDOM_REPLY:
		onSearchRandomReply(in);
		break;

	case TCP_CMD_SEARCH_UIN_REPLY:
		onSearchUINReply(in);
		break;
	}
}
