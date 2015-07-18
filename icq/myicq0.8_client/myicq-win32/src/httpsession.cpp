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

#include "httpsession.h"
#include "sessionlistener.h"
#include "icqsocket.h"
#include "icqclient.h"
#include "udppacket.h"
#include "base64.h"

#define	MYICQ_HTTP_PORT		443

enum {
	HTTP_STATUS_CONNECT,
	HTTP_STATUS_ESTABLISHED
};

HttpSession::HttpSession()
{
	sock = -1;
	status = -1;
	listener = NULL;

	bufSize = 0;
}

HttpSession::~HttpSession()
{
	if (sock >= 0)
		IcqSocket::closeSocket(sock);
}

bool HttpSession::start(const char *destHost, ProxyInfo &proxy)
{
	this->destHost = destHost;
	bufSize = 0;

	if (sock >= 0)
		IcqSocket::closeSocket(sock);
	sock = IcqSocket::createSocket(SOCK_STREAM, this);
	
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;

	uname = proxy.username;
	passwd = proxy.passwd;

	const char *host = proxy.host.c_str();
	if ((addr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		hostent *he = gethostbyname(host);
		if (!he)
			goto failed;
		addr.sin_addr = *(in_addr *) he->h_addr;
	}
	addr.sin_port = htons(proxy.port);

	status = HTTP_STATUS_CONNECT;

	if (connect(sock, (sockaddr *) &addr, sizeof(addr)) == 0)
		onConnect(true);
	else if (IcqSocket::getSocketError() != EINPROGRESS)
		goto failed;

	return true;
failed:
	close(sock);
	sock = -1;
	listener->sessionFinished(false);
	return false;
}

void HttpSession::onConnect(bool success)
{
	if (!success) {
		listener->sessionFinished(false);
		return;
	}

	string request = "CONNECT ";
	request += destHost + ":" + itoa(MYICQ_HTTP_PORT, buf, 10);
	request += " HTTP/1.0\r\n";
	request += "User-agent: MyICQ\r\n";

	if (!uname.empty()) {
		string str = uname + ":" + passwd;
		char *code = NULL;
		encode_base64(str.c_str(), str.length(), &code);
		if (code) {
			request += "Proxy-authorization: Basic ";
			request += code;
			request += "\r\n";

			free(code);
		}
	}

	request += "\r\n";
	send(sock, request.c_str(), request.length(), 0);
}

bool HttpSession::onReceive()
{
	int n;

	if (status == HTTP_STATUS_CONNECT) {
		n = recv(sock, buf, sizeof(buf), 0);
		if (n <= 0)
			goto failed;

		buf[n] = '\0';

		char *p1 = strchr(buf, ' ');
		char *p2 = strchr(buf, '\r');
		if (!p1 || !p2)
			goto failed;

		++p1;
		if (strnicmp(p1, "200 Connection established", p2 - p1) != 0)
			goto failed;

		status = HTTP_STATUS_ESTABLISHED;
		listener->sessionFinished(true);

	} else if (status == HTTP_STATUS_ESTABLISHED)
		return recvPacket();

	return true;
failed:
	IcqSocket::closeSocket(sock);
	sock = -1;
	listener->sessionFinished(false);
	return false;
}

void HttpSession::onClose()
{
}

bool HttpSession::recvPacket()
{
	int n;
	while ((n = recv(sock, buf + bufSize, sizeof(buf) - bufSize, 0)) > 0) {
		bufSize += n;
		char *start = buf;
		char *end = start + bufSize;
		uint16 len;

		while (start + sizeof(len) < end) {
			len = ntohs(*(uint16 *) start);
			if (end - start - sizeof(len) < len)
				break;

			start += sizeof(len);
			if (len >= sizeof(UDP_HEADER)) {
				UdpInPacket in(start, len);
				listener->onPacketReceived(in);
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
