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

#include "sockssession.h"
#include "sessionlistener.h"
#include "icqsocket.h"
#include "icqclient.h"

enum {
	SOCKS_STATUS_METHOD,
	SOCKS_STATUS_AUTH,
	SOCKS_STATUS_ASSOCIATE,
	SOCKS_STATUS_FINISHED
};


SocksSession::SocksSession()
{
	sock = -1;
	status = -1;
	socksIP = 0;
	socksPort = 
	localPort = 0;
	listener = NULL;
}

SocksSession::~SocksSession()
{
	if (sock >= 0)
		IcqSocket::closeSocket(sock);
}

bool SocksSession::start(ProxyInfo &socks, uint16 port)
{
	localPort = port;
	uname = socks.username.c_str();
	passwd = socks.passwd.c_str();
	status = -1;

	if (sock >= 0)
		IcqSocket::closeSocket(sock);
	sock = IcqSocket::createSocket(SOCK_STREAM, this);

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(socks.port);

	const char *host = socks.host.c_str();
	if ((addr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		hostent *hent = gethostbyname(host);
		if (!hent)
			goto failed;
		addr.sin_addr = *(in_addr *) hent->h_addr;
	}
	if (::connect(sock, (sockaddr *) &addr, sizeof(addr)) == 0)
		onConnect(true);
	else if (IcqSocket::getSocketError() != EINPROGRESS)
		goto failed;

	return true;

failed:
	listener->sessionFinished(false);
	return false;
}

void SocksSession::onConnect(bool success)
{
	if (!success) {
		listener->sessionFinished(false);
		return;
	}
	
	char buf[3];

	buf[0] = 5;
	buf[1] = 1;
	buf[2] = (uname.empty() ? 0 : 2);
	send(sock, buf, 3, 0);

	status = SOCKS_STATUS_METHOD;
}

bool SocksSession::onReceive()
{
	sockaddr_in addr;
	char buf[1024];

	int n = recv(sock, buf, sizeof(buf), 0);

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
			send(sock, buf, p - buf, 0);
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

		socksIP = *(uint32 *) &buf[4];
		socksPort = *(uint16 *) &buf[8];
		listener->sessionFinished(true);
		break;

	default:
		goto failed;
	}

	if (status == SOCKS_STATUS_ASSOCIATE) {
		if (!localPort) {
			status = SOCKS_STATUS_FINISHED;
			listener->sessionFinished(true);
		} else {
			buf[0] = 5;
			buf[1] = 3;
			buf[2] = 0;
			buf[3] = 1;
			*(uint32 *) &buf[4] = 0;
			*(uint16 *) &buf[8] = localPort;
			send(sock, buf, 10, 0);
		}
	}
	return true;

failed:
	IcqSocket::closeSocket(sock);
	sock = -1;
	listener->sessionFinished(false);
	return false;
}

void SocksSession::onClose()
{
	listener->sessionFinished(false);
}
