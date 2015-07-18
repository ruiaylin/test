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

#ifndef _UDP_SESSION_H_
#define _UDP_SESSION_H_

#include "icqlist.h"
#include "udppacket.h"
#include "icqsocket.h"
#include <mysql.h>

#define KEEPALIVE_TIMEOUT	(3 * 60)
#define SEND_TIMEOUT		10


class UdpSession {
friend class SessionHash;
public:
	UdpSession();
	~UdpSession();

	bool onReceive(UdpInPacket &);

	static bool initialize();
	static void cleanUp();
	static bool onReceive();
	static time_t checkSendQueue();
	static time_t checkKeepAlive();
	static inline void sendMessage(uint8 type, uint32 dst, uint32 src, UdpSession *dstSession, time_t when, const char *text);
	static void broadcastMessages();

private:
	void sendDirect(UdpOutPacket *p) {
		p->send(sock, ip, port);
	}
	void createPacket(UdpOutPacket &out, uint16 cmd, uint16 ackSeq);
	UdpOutPacket *createPacket(uint16 cmd, uint16 ackSeq = 0);
	void sendPacket(UdpOutPacket *);
	void sendAckPacket(uint16 cmd, uint16 seq);
	void sendSearchReply(int n, uint16 ackSeq, bool online);
	bool setWindow(uint16 seq);
	void dead();

	void onlineNotify();
	void offlineNotify();
	inline void addFriend(uint32 dst, uint32 src, UdpSession *dstSession, UdpSession *srcSession);
	void sendMessages(const char *sql, int n);

	void onAck(uint16 seq);
	void onKeepAlive(UdpInPacket &);
	void onNewUIN(UdpInPacket &);
	void onGetContactList(UdpInPacket &in);
	void onLogin(UdpInPacket &);
	void onLogout(UdpInPacket &);
	void onChangeStatus(UdpInPacket &);
	void onUpdateContact(UdpInPacket &);
	void onModifyUser(UdpInPacket &);
	void onUpdateUser(UdpInPacket &);
	void onSendMessage(UdpInPacket &);
	void onGroupSendMessage(UdpInPacket &in);
	void onSearchRandom(UdpInPacket &);
	void onSearchCustom(UdpInPacket &);
	void onAddFriend(UdpInPacket &);
	void onDelFriend(UdpInPacket &);
	void onBroadcastMsg(UdpInPacket &);

	uint16 udpVer;
	uint16 tcpVer;
	uint32 sid;
	uint32 uin;
	uint8 auth;
	uint32 ip;
	uint16 port;
	uint32 realIP;
	uint32 status;
	char passwd[8];
	
	uint16 sendSeq;
	uint16 recvSeq;
	uint32 window;
	time_t expire;

	IcqListItem listItem;
	IcqListItem keepAliveItem;
	IcqList sendQueue;

	static IcqList globalSendQueue;
	static IcqList keepAliveList;
	static IcqList broadMsgList;
	static int sock;
	static MYSQL mysql;
	static char sqlStmt[];
	static uint32 sessionCount;
};

#endif
