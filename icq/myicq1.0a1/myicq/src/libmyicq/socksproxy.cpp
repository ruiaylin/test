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

#include "socksproxy.h"
#include "proxy.h"

enum {
	SOCKS_STATUS_NOT_CONN = -1,
	SOCKS_STATUS_METHOD,
	SOCKS_STATUS_AUTH,
	SOCKS_STATUS_ASSOCIATE,
	SOCKS_STATUS_FINISHED
};


SocksProxy::SocksProxy()
{
	tcpSock = -1;
	status = -1;
	listener = NULL;
}

SocksProxy::~SocksProxy()
{
	if (tcpSock >= 0)
		IcqSocket::closeSocket(tcpSock);
}

bool SocksProxy::start(ProxyInfo &info, int sock)
{
	udpSock = sock;
	uname = info.username.c_str();
	passwd = info.passwd.c_str();
	status = SOCKS_STATUS_NOT_CONN;

	if (tcpSock >= 0)
		IcqSocket::closeSocket(tcpSock);
	tcpSock = IcqSocket::createSocket(SOCK_STREAM, this);

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(info.port);

	const char *host = info.host.c_str();
	if ((addr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		hostent *hent = gethostbyname(host);
		if (!hent)
			goto failed;
		addr.sin_addr = *(in_addr *) hent->h_addr;
	}
	if (::connect(tcpSock, (sockaddr *) &addr, sizeof(addr)) == 0)
		onConnect(true);
	else if (IcqSocket::getSocketError() != EINPROGRESS)
		goto failed;

	return true;

failed:
	if (tcpSock >= 0) {
		IcqSocket::closeSocket(tcpSock);
		tcpSock = -1;
	}
	listener->onEstablished(false);
	return false;
}

void SocksProxy::onConnect(bool success)
{
	if (!success) {
		listener->onEstablished(false);
		return;
	}
	
	char buf[3];

	buf[0] = 5;
	buf[1] = 1;
	buf[2] = (uname.empty() ? 0 : 2);
	send(tcpSock, buf, 3, 0);

	status = SOCKS_STATUS_METHOD;
}

bool SocksProxy::onReceive()
{
	char buf[1024];
	int n = recv(tcpSock, buf, sizeof(buf), 0);

	switch (status) {
	case SOCKS_STATUS_METHOD:
		if (n != 2 || buf[0] != 5 || (buf[1] != 0 && buf[1] != 2))
			goto failed;
		if (buf[1] == 0)
			status = SOCKS_STATUS_ASSOCIATE;
		else {
			status = SOCKS_STATUS_AUTH;

			char *p = buf;
			*p++ = 1;
			*p++ = n = uname.length();
			memcpy(p, uname.c_str(), n);
			p += n;
			*p++ = n = passwd.length();
			memcpy(p, passwd.c_str(), n);
			p += n;
			send(tcpSock, buf, p - buf, 0);
		}
		break;

	case SOCKS_STATUS_AUTH:
		if (n != 2 || buf[0] != 1 || buf[1] != 0)
			goto failed;
		status = SOCKS_STATUS_ASSOCIATE;
		break;

	case SOCKS_STATUS_ASSOCIATE:
		if (n != 10 || buf[0] != 5 || buf[1] != 0 || buf[3] != 1)
			goto failed;

		status = SOCKS_STATUS_FINISHED;

		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr = *(in_addr *) &buf[4];
		addr.sin_port = *(uint16 *) &buf[8];
		connect(udpSock, (sockaddr *) &addr, sizeof(addr));

		listener->onEstablished(true);
		break;

	default:
		goto failed;
	}

	if (status == SOCKS_STATUS_ASSOCIATE) {
		if (udpSock < 0) {
			status = SOCKS_STATUS_FINISHED;
			listener->onEstablished(true);
		} else {
			sockaddr_in addr;
			socklen_t len = sizeof(addr);

			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = INADDR_ANY;
			addr.sin_port = 0;
			bind(udpSock, (sockaddr *) &addr, len);

			getsockname(udpSock, (sockaddr *) &addr, &len);

			buf[0] = 5;
			buf[1] = 3;
			buf[2] = 0;
			buf[3] = 1;
			*(in_addr *) &buf[4] = addr.sin_addr;
			*(uint16 *) &buf[8] = addr.sin_port;
			send(tcpSock, buf, 10, 0);
		}
	}
	return true;

failed:
	IcqSocket::closeSocket(tcpSock);
	tcpSock = -1;
	listener->onEstablished(false);
	return false;
}

void SocksProxy::onClose()
{
	listener->onEstablished(false);
}
