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

#ifndef _SERVER_H
#define _SERVER_H

#include "refobject.h"
#include "session.h"
#include "list.h"
#include "slab.h"
#include "tcppacket.h"
#include "icqsocket.h"
#include "dbmanager.h"
#include <time.h>

#define MAX_DOMAIN_NAME		64
#define MAX_DESC_NAME		16
#define UIN_HASH_SIZE		4096


class RemoteSession : public Session {
public:
	RemoteSession(uint32 uin) {
		this->uin = uin;
	}

	ListHead hashItem;
	ListHead listItem;

	DECLARE_SLAB(RemoteSession)
};


class TcpOutPacket;
class TcpInPacket;

class Server : public RefObject {
public:
	Server();
	virtual ~Server();

	void onDnsResolved(uint32 ip);
	void onConnect();
	void onReceive();
	void onClose();

	bool connect(uint32 ip, uint16 port);
	RemoteSession *getSession(uint32 uin);

	void sendUserOnline(Session *s);
	void sendUserOffline(uint32 uin);
	void sendUserStatus(uint32 uin, uint32 status);
	void sendMessage(uint8 type, uint32 dst, uint32 src, ICQ_STR &text);
	void addFriend(uint16 seq, uint32 dst, uint32 src);
	void delFriend(uint32 dst, uint32 src);
	void updateContact(uint16 seq, uint32 dst, uint32 src);
	void searchRandom(uint16 seq, uint32 src);
	void searchUIN(uint16 seq, uint32 dst, uint32 src);

	void addFriendReply(uint16 seq, uint32 dst, uint32 src, uint8 auth);
	void updateContactReply(uint16 seq, uint32 src, uint8 *data, int n);
	void searchUINReply(uint16 seq, uint32 src, uint8 *data, int n);

	static bool init();
	static void destroy();
	static Server *getServer(const char *name);
	static Server *createServer(const char *name);
	static RemoteSession *getSession(QID &qid);
	static int generateFds(fd_set &readfds, fd_set &writefds);
	static void examineFds(fd_set &readfds, fd_set &writefds);

	static ListHead serverList;

	char domainName[MAX_DOMAIN_NAME + 1];
	char descName[MAX_DESC_NAME + 1];
	ICQ_STR domain;
	ICQ_STR desc;

	uint32 ip;
	int sock;
	int status;
	time_t expire;
	uint32 sessionCount;
	bool isAccepted;

	ListHead listItem;
	ListHead hashItem;

private:
	bool setDomainAndDesc(ICQ_STR &name, ICQ_STR &des);

	RemoteSession *createSession(uint32 uin);
	void notify(uint32 uin, DB_CALLBACK cb);

	void createPacket(TcpOutPacket &out, uint16 cmd);
	TcpOutPacket *createPacket(uint16 cmd);
	void sendPacket(TcpOutPacket *out);
	void sendPendingPacket(int s);

	void onPacketReceived(TcpInPacket &in);
	void onHello(TcpInPacket &in);
	void onSendMessage(TcpInPacket &in);
	void onAddFriend(TcpInPacket &in);
	void onDelFriend(TcpInPacket &in);
	void onUserOnline(TcpInPacket &in);
	void onUserOffline(TcpInPacket &in);
	void onUserStatus(TcpInPacket &in);
	void onUpdateContact(TcpInPacket &in);
	void onSearchRandom(TcpInPacket &in);
	void onSearchUIN(TcpInPacket&in);

	void onHelloReply(TcpInPacket &in);
	void onAddFriendReply(TcpInPacket &in);
	void onUpdateContactReply(TcpInPacket &in);
	void onSearchRandomReply(TcpInPacket &in);
	void onSearchUINReply(TcpInPacket &in);

	char buf[TCP_PACKET_SIZE];
	int bufSize;

	ListHead uinHash[UIN_HASH_SIZE];
	ListHead sessionList;
	ListHead sendQueue;

	DECLARE_SLAB(Server)
};

#endif
