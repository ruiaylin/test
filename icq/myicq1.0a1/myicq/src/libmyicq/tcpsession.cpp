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

#include "icqlink.h"
#include "tcppacket.h"
#include "tcpsession.h"
#include "icqsocket.h"
#include "icqplugin.h"


TcpSession::TcpSession(IcqLink *link, const char *name, QID &qid)
{
	icqLink = link;
	this->name = name;
	this->qid = qid;
	
	listener = NULL;
	isSend = true;
	useSocks = false;

	sock = -1;
	destIP = 0;
	destPort = 0;

	recvBufSize = sendBufSize = 0;
	status = TCP_STATUS_NOT_CONN;

	contact = link->findContact(qid);
	realIP = (contact ? contact->realIP : 0);
}

TcpSession::~TcpSession()
{
	shutdown();
}

void TcpSession::clearSendQueue()
{
	PtrList::iterator it;
	for (it = sendQueue.begin(); it != sendQueue.end(); ++it)
		delete (OutPacket *) *it;
	sendQueue.clear();
}

void TcpSession::deleteIt()
{
	if (listener)
		listener->onClose(false);
	delete this;
}

IcqLinkBase *TcpSession::getLink()
{
	return icqLink;
}

void TcpSession::shutdown()
{
	clearSendQueue();

	if (sock >= 0) {
		IcqSocket::closeSocket(sock);
		sock = -1;
	}
}

void TcpSession::destroy()
{
	icqLink->removeTcpSession(this);
	delete this;
}

/*
 * Create a new session of this plugin
 */
void TcpSession::createPlugin()
{
	IcqPlugin *p = PluginFactory::getPlugin(name);
	if (p && p->info.type == ICQ_PLUGIN_NET)
		listener = ((NetPlugin *) p)->createSession(this);
}

bool TcpSession::connect(uint32 ip, uint16 port, bool isSend)
{
	destIP = ip;
	destPort = port;
	this->isSend = isSend;

	status = TCP_STATUS_DISABLED;

	if (icqLink->ourIP == ip) {
		// if we are behind the same firewall, connect directly
		ip = realIP;

	} else if (realIP == ip) {
		if (icqLink->isProxyType(PROXY_SOCKS)) {
			useSocks = true;
			ip = icqLink->ourIP;
			port = icqLink->options.proxy[PROXY_SOCKS].port;
		} else if (icqLink->isProxyType(PROXY_HTTP))
			return false;

	} else {
		// Unfortunately, the remote is behind a firewall,
		// so that the connection can not be established
		return false;
	}

	sock = IcqSocket::createSocket(SOCK_STREAM, this);

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	if (::connect(sock, (sockaddr *) &addr, sizeof(addr)) == 0)
		onConnect(true);
	else if (IcqSocket::getSocketError() != EINPROGRESS)
		return false;

	status = TCP_STATUS_NOT_CONN;
	return true;
}

void TcpSession::enableWrite()
{
	IcqSocket::enableWrite(sock);
}

OutPacket *TcpSession::createPacket(uint16 cmd)
{
	ContactInfo &info = icqLink->myInfo;

	OutPacket *p = new TcpOutPacket;
	*p << (uint16) MYICQ_TCP_VER << (uint32) 0;
	*p << cmd << info.qid.uin << info.face;
	return p;
}

bool TcpSession::sendPacket(OutPacket *out)
{
	if (status != TCP_STATUS_ESTABLISHED) {
		sendQueue.push_back(out);
		return true;
	}

	TcpOutPacket *p = (TcpOutPacket *) out;
	int bytesTotal = p->getSize();
	int bytesSent = p->send(sock);
	int n = bytesTotal - bytesSent;

	if (n > 0) {
		if (n > sizeof(sendBuf) - sendBufSize)
			return false;

		memcpy(sendBuf + sendBufSize, p->getData() + bytesSent, n);
		sendBufSize += n;
	}

	delete p;
	return true;
}

void TcpSession::sendHello()
{
	OutPacket *out = createPacket(TCP_HELLO);
	*out << (uint8) isSend;
	if (!qid.domain.empty())
		*out << icqLink->myInfo.qid.domain;
	sendPacket(out);
}

/*
 * Called when the TCP session is _really established
 */
bool TcpSession::onTcpEstablished()
{
	status = TCP_STATUS_ESTABLISHED;

	// Hello packet is the first packet that sent between two tcp clients.
	sendHello();

	while (!sendQueue.empty()) {
		OutPacket *out = (OutPacket *) sendQueue.front();
		sendQueue.pop_front();
		sendPacket(out);
	}

	createPlugin();
	return true;
}

void TcpSession::onConnect(bool success)
{
	if (!success) {
		status = TCP_STATUS_DISABLED;
		return;
	}

	if (!useSocks) {
		onTcpEstablished();
		return;
	}

	status = TCP_STATUS_SOCKS_METHOD;

	recvBuf[0] = 5;
	recvBuf[1] = 1;
	recvBuf[2] = (icqLink->options.proxy[PROXY_SOCKS].username.empty() ? 0 : 2);
	send(sock, recvBuf, 3, 0);
}

bool TcpSession::recvPacket()
{
	// Since TCP data stream has no concept of packets, we must do it ourselves.
	int n;
	while ((n = recv(sock, recvBuf + recvBufSize, sizeof(recvBuf) - recvBufSize, 0)) > 0) {
		recvBufSize += n;
		char *start = recvBuf;
		char *end = start + recvBufSize;
		uint16 len;

		while (start + sizeof(len) < end) {
			len = ntohs(*(uint16 *) start);
			if (end - start - sizeof(len) < len) {
				// Not a complete packet
				break;
			}

			start += sizeof(len);
			if (len >= sizeof(TCP_HEADER)) {
				TcpInPacket in(start, len);
				onPacketReceived(in);
			}
			start += len;
		}

		recvBufSize = end - start;
		if (recvBufSize > 0)
			memcpy(recvBuf, start, recvBufSize);
	}
	if (n == 0)
		onClose();

	return (n >= 0);
}

bool TcpSession::onReceive()
{
	if (status == TCP_STATUS_ESTABLISHED || status == TCP_STATUS_HELLO_WAIT)
		return recvPacket();

	int n = recv(sock, recvBuf, sizeof(recvBuf), 0);

	switch (status) {
	case TCP_STATUS_SOCKS_METHOD:
		if (n != 2 || recvBuf[0] != 5 || (recvBuf[1] != 0 && recvBuf[1] != 2))
			return false;
		if (recvBuf[1] == 0)
			status = TCP_STATUS_SOCKS_CONNECT;
		else {
			status = TCP_STATUS_SOCKS_AUTH;

			ProxyInfo &socks = icqLink->options.proxy[PROXY_SOCKS];
			char *p = recvBuf;
			*p++ = 1;
			*p++ = n = socks.username.length();
			memcpy(p, socks.username.c_str(), n);
			p += n;
			*p++ = n = socks.passwd.length();
			memcpy(p, socks.passwd.c_str(), n);
			p += n;
			send(sock, recvBuf, p - recvBuf, 0);
		}
		break;

	case TCP_STATUS_SOCKS_AUTH:
		if (n != 2 || recvBuf[0] != 1 || recvBuf[1] != 0)
			return false;
		status = TCP_STATUS_SOCKS_CONNECT;
		break;

	case TCP_STATUS_SOCKS_CONNECT:
		if (n != 10 || recvBuf[0] != 5 || recvBuf[1] != 0)
			return false;
		onTcpEstablished();
		break;

	default:
		return false;
	}

	if (status == TCP_STATUS_SOCKS_CONNECT) {
		recvBuf[0] = 5;
		recvBuf[1] = 1;
		recvBuf[2] = 0;
		recvBuf[3] = 1;
		*(uint32 *) &recvBuf[4] = htonl(destIP);
		*(uint16 *) &recvBuf[8] = htons(destPort);
		send(sock, recvBuf, 10, 0);
	}
	return true;
}

void TcpSession::onSend()
{
	if (status != TCP_STATUS_ESTABLISHED)
		return;

	if (sendBufSize > 0) {
		int n = send(sock, sendBuf, sendBufSize, 0);
		sendBufSize -= n;
		if (sendBufSize > 0)
			memcpy(sendBuf, sendBuf + n, sendBufSize);
	}

	if (!sendBufSize)
		listener->onSend();
}

void TcpSession::onClose()
{
	shutdown();

	icqLink->removeTcpSession(this);
	listener->onClose(true);
	delete this;
}

void TcpSession::onHello(TcpInPacket &in)
{
	if (status != TCP_STATUS_HELLO_WAIT)
		return;

	status = TCP_STATUS_ESTABLISHED;
	qid.uin = in.header.uin;

	uint8 b;
	in >> b >> qid.domain;
	isSend = !b;

	contact = icqLink->findContact(qid);

	createPlugin();
}

bool TcpSession::onPacketReceived(TcpInPacket &in)
{
	uint16 cmd = in.header.cmd;

	if (cmd == TCP_HELLO)
		onHello(in);
	else if (status == TCP_STATUS_HELLO_WAIT || qid.uin != in.header.uin)
		return false;

	if (contact)
		contact->face = in.header.face;

	return listener->onPacketReceived(in);
}
