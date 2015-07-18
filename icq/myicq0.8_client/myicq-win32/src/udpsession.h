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

#ifndef _UDP_SESSION_H
#define _UDP_SESSION_H

#include "icqsession.h"
#include "icqsocket.h"

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

	UDP_MSG_FIRST = 0x0200,
	UDP_MSG_MESSAGE = UDP_MSG_FIRST,
	UDP_MSG_MESSAGE_ACK,
};

#define KEEPALIVE_TIMEOUT		(2 * 60)
#define SEND_TIMEOUT			10

class IcqLink;
class UdpOutPacket;
class UdpInPacket;

class UdpSession : public IcqSession {
public:
	UdpSession(IcqLink *link, const char *name, uint32 uin);
	virtual ~UdpSession();

	virtual bool onReceive();

	void connect(uint32 ip, uint16 port);
	time_t checkSendQueue();

protected:
	virtual bool onPacketReceived(UdpInPacket &in);
	virtual void sendDirect(UdpOutPacket *p);
	virtual void onSendError(UdpOutPacket *p);

	void sendDirect(UdpOutPacket *out, int s, sockaddr_in *addr = NULL);
	bool onAck(uint16 seq);
	void clearSendQueue();
	void initSession();
	void createPacket(UdpOutPacket &out, uint16 cmd, uint16 seq, uint16 ackseq = 0);
	UdpOutPacket *createPacket(uint16 cmd, uint16 ackseq = 0);
	void sendAckPacket(uint16 seq);
	uint16 sendPacket(UdpOutPacket *);
	bool setWindow(uint16 seq);

	static int udpSock;				// initialized by ServerSession
	static string destHost;
	static sockaddr_in proxyAddr;

	uint32 sid;
	uint16 sendSeq;
	uint16 recvSeq;
	uint32 window;
	PtrList sendQueue;

	sockaddr_in destAddr;
};

#endif
