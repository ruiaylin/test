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

#ifndef _TCP_SESSION_H
#define _TCP_SESSION_H

#include "tcpsessionbase.h"
#include "icqsocket.h"

#define TCP_PACKET_SIZE	4096
#define TCP_HELLO		1

// TCP Session status
enum {
	TCP_STATUS_DISABLED,
	TCP_STATUS_NOT_CONN,
	TCP_STATUS_SOCKS_METHOD,
	TCP_STATUS_SOCKS_AUTH,
	TCP_STATUS_SOCKS_CONNECT,
	TCP_STATUS_HELLO_WAIT,
	TCP_STATUS_ESTABLISHED,
};

class IcqLink;
class TcpInPacket;

class TcpSession : public TcpSessionBase, public SocketListener {
public:
	TcpSession(IcqLink *link, const char *name, QID &qid);

	void clearSendQueue();
	void deleteIt();

	// TcpSessinBase interfaces...
	virtual IcqLinkBase *getLink();
	virtual int getSocket() {
		return sock;
	}
	virtual void getRemoteQID(QID &qid) {
		qid = this->qid;
	}
	virtual bool isSendSession() {
		return isSend;
	}
	virtual void destroy();
	virtual void shutdown();
	virtual void enableWrite();
	virtual OutPacket *createPacket(uint16 cmd);
	virtual bool sendPacket(OutPacket *out);

	// SocketListener interfaces...
	virtual void onConnect(bool success);
	virtual bool onReceive();
	virtual void onSend();
	virtual void onClose();

	bool connect(uint32 ip, uint16 port, bool isSend);

	QID qid;
	string name;
	IcqLink *icqLink;
	IcqContact *contact;
	int sock;
	int status;

	TcpSessionListener *listener;

private:
	virtual ~TcpSession();

	bool onPacketReceived(TcpInPacket &in);

	void sendHello();
	void createPlugin();
	bool recvPacket();
	bool onTcpEstablished();
	void onHello(TcpInPacket &in);

	bool isSend;
	bool useSocks;
	uint32 destIP;
	uint16 destPort;
	uint32 realIP;
	PtrList sendQueue;

	char recvBuf[TCP_PACKET_SIZE];
	int recvBufSize;
	char sendBuf[TCP_PACKET_SIZE];
	int sendBufSize;
};

#endif
