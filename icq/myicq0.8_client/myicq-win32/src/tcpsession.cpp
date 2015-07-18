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


TcpSession::TcpSession(IcqLink *link, const char *name, uint32 uin)
	: IcqSession(link, name, uin)
{
	listener = NULL;
	isSend = true;

	sock = -1;
	destIP = 0;
	destPort = 0;

	bufSize = 0;
	status = TCP_STATUS_NOT_CONN;

	IcqContact *c = link->findContact(uin);
	realIP = (c ? c->realIP : 0);
}

TcpSession::~TcpSession()
{
	if (sock >= 0)
		IcqSocket::closeSocket(sock);
}

void TcpSession::shutdown()
{
	::shutdown(sock, 2);
}

void TcpSession::destroy()
{
	if (name.empty())
		delete this;
	else
		icqLink->destroySession(name.c_str(), uin);
}

void TcpSession::createPlugin()
{
	IcqPlugin *p = PluginFactory::getPlugin(name);
	if (p && p->type == ICQ_PLUGIN_NET)
		listener = ((NetPlugin *) p)->createSession(this);
}

bool TcpSession::connect(uint32 ip, uint16 port, bool isSend)
{
	destIP = ip;
	destPort = port;
	this->isSend = isSend;

	sock = IcqSocket::createSocket(SOCK_STREAM, this);
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;

	IcqOption &options = icqLink->options;
	
	if (icqLink->isProxyType(PROXY_SOCKS)) {
		if (realIP && icqLink->socksIP == ip)
			ip = realIP;		// We are behind the same firewall.
		else {
			ip = icqLink->socksIP;
			port = icqLink->options.proxy[PROXY_SOCKS].port;
		}
	}

	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	if (::connect(sock, (sockaddr *) &addr, sizeof(addr)) == 0)
		onConnect(true);
	else if (IcqSocket::getSocketError() != EINPROGRESS)
		return false;

	return true;
}

void TcpSession::enableWrite()
{
	IcqSocket::enableWrite(sock);
}

OutPacket *TcpSession::createPacket(uint16 cmd)
{
	OutPacket *p = new TcpOutPacket;
	*p << (uint16) MYICQ_TCP_VER << (uint32) 0;
	*p << cmd << icqLink->myInfo.uin;
	return p;
}

bool TcpSession::sendPacket(OutPacket *out)
{
	if (status == TCP_STATUS_ESTABLISHED) {
		int n = sendDirect(out);
		delete out;
		return (n > 0);
	}
	return false;
}

int TcpSession::sendDirect(OutPacket *out)
{
	return ((TcpOutPacket *) out)->send(sock);
}

bool TcpSession::onTcpEstablished()
{
	status = TCP_STATUS_ESTABLISHED;

	if (listener && listener->onTcpEstablished())
		return true;

	OutPacket *p = createPacket(TCP_HELLO);
	*p << (uint8) isSend;
	sendDirect(p);
	delete p;

	createPlugin();
	return true;
}

void TcpSession::onConnect(bool success)
{
	if (!success)
		return;

	if (!icqLink->isProxyType(PROXY_SOCKS) || 
		(realIP && icqLink->socksIP == destIP)) {
		onTcpEstablished();
		return;
	}

	status = TCP_STATUS_SOCKS_METHOD;

	buf[0] = 5;
	buf[1] = 1;
	buf[2] = (icqLink->options.proxy[PROXY_SOCKS].username.empty() ? 0 : 2);
	send(sock, buf, 3, 0);
}

bool TcpSession::recvPacket()
{
	if (listener && listener->onReceive())
		return true;

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
	if (n == 0)
		onClose();

	return (n >= 0);
}

bool TcpSession::onReceive()
{
	if (status == TCP_STATUS_ESTABLISHED || status == TCP_STATUS_HELLO_WAIT)
		return recvPacket();

	int n = recv(sock, buf, sizeof(buf), 0);

	switch (status) {
	case TCP_STATUS_SOCKS_METHOD:
		if (n != 2 || buf[0] != 5 || (buf[1] != 0 && buf[1] != 2))
			return false;
		if (buf[1] == 0)
			status = TCP_STATUS_SOCKS_CONNECT;
		else {
			status = TCP_STATUS_SOCKS_AUTH;

			ProxyInfo &socks = icqLink->options.proxy[PROXY_SOCKS];
			char *p = buf;
			*p++ = 1;
			*p++ = n = socks.username.length();
			memcpy(p, socks.username.c_str(), n);
			p += n;
			*p++ = n = socks.passwd.length();
			memcpy(p, socks.passwd.c_str(), n);
			p += n;
			send(sock, buf, p - buf, 0);
		}
		break;

	case TCP_STATUS_SOCKS_AUTH:
		if (n != 2 || buf[0] != 1 || buf[1] != 0)
			return false;
		status = TCP_STATUS_SOCKS_CONNECT;
		break;

	case TCP_STATUS_SOCKS_CONNECT:
		if (n != 10 || buf[0] != 5 || buf[1] != 0)
			return false;
		onTcpEstablished();
		break;

	default:
		return false;
	}

	if (status == TCP_STATUS_SOCKS_CONNECT) {
		buf[0] = 5;
		buf[1] = 1;
		buf[2] = 0;
		buf[3] = 1;
		*(uint32 *) &buf[4] = htonl(destIP);
		*(uint16 *) &buf[8] = htons(destPort);
		send(sock, buf, 10, 0);
	}
	return true;
}

void TcpSession::onSend()
{
	if (status == TCP_STATUS_ESTABLISHED)
		listener->onSend();
}

void TcpSession::onClose()
{
	listener->onClose();
}

bool TcpSession::onPacketReceived(TcpInPacket &in)
{
	uint16 cmd = in.getCmd();
	if (cmd == TCP_HELLO) {
		if (status != TCP_STATUS_HELLO_WAIT)
			return false;

		status = TCP_STATUS_ESTABLISHED;
		uin = in.getUIN();

		uint8 b;
		in >> b;
		isSend = !b;

		if (!listener)
			createPlugin();

	} else if (status == TCP_STATUS_HELLO_WAIT || uin != in.getUIN())
			return false;

	return listener->onPacketReceived(in);
}
