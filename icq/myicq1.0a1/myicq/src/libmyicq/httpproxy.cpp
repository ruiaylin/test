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

#include "httpproxy.h"
#include "proxy.h"
#include "udppacket.h"
#include "base64.h"

// Default HTTPS port that almost all http proxies should open, I think:-)
#define	MYICQ_HTTP_PORT		443

enum {
	HTTP_STATUS_CONNECT,
	HTTP_STATUS_ESTABLISHED
};

HttpProxy::HttpProxy()
{
	sock = -1;
	status = -1;
	listener = NULL;

	bufSize = 0;
}

HttpProxy::~HttpProxy()
{
	if (sock >= 0)
		IcqSocket::closeSocket(sock);
}

/*
 * Start an http proxy session
 */
bool HttpProxy::start(const char *destHost, ProxyInfo &proxy)
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

	// Waiting for connection
	status = HTTP_STATUS_CONNECT;

	if (connect(sock, (sockaddr *) &addr, sizeof(addr)) == 0)
		onConnect(true);
	else if (IcqSocket::getSocketError() != EINPROGRESS)
		goto failed;

	return true;
failed:
	if (sock >= 0) {
		IcqSocket::closeSocket(sock);
		sock = -1;
	}
	listener->onEstablished(false);
	return false;
}

void HttpProxy::onConnect(bool success)
{
	if (!success) {
		listener->onEstablished(false);
		return;
	}

	// Send 'CONNECT' request
	string request = "CONNECT ";
	sprintf(buf, "%d", MYICQ_HTTP_PORT);
	request += destHost + ":" + buf;
	request += " HTTP/1.0\r\n";
	request += "User-agent: MyICQ\r\n";

	if (!uname.empty()) {
		// Encode using base64
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

bool HttpProxy::onReceive()
{
	// Since we are already connected, receive real packet data
	if (status == HTTP_STATUS_ESTABLISHED)
		return recvPacket();

	if (status == HTTP_STATUS_CONNECT) {
		int n = recv(sock, buf, sizeof(buf), 0);
		if (n <= 0)
			goto failed;

		buf[n] = '\0';

		FILE *file = fopen("c:\\myicq-http.log", "a+");
		if (file) {
			fprintf(file, "%s\n\n", buf);
			fclose(file);
		}

		char *seps = " ";
		char *p = strtok(buf, seps);
		if (p)
			p = strtok(NULL, seps);
		if (!p || *p != '2')
			goto failed;

		// Wow, the proxy has successfully established a tcp connection to myicqhttp
		status = HTTP_STATUS_ESTABLISHED;
		listener->onEstablished(true);
	}
	return true;

failed:
	IcqSocket::closeSocket(sock);
	sock = -1;
	listener->onEstablished(false);
	return false;
}

void HttpProxy::onClose()
{
	// The proxy has closed this connection, and the only thing
	// we can do now is to just notify the listener about this.
	listener->onEstablished(false);
}

bool HttpProxy::recvPacket()
{
	// Since TCP connection is really a data stream and there is no packet boundary,
	// extra things must be done. we put an extra 2 bytes(WORD) at the head of each
	// packet describing its size.
	
	int n;
	while ((n = recv(sock, buf + bufSize, sizeof(buf) - bufSize, 0)) > 0) {
		bufSize += n;
		char *start = buf;
		char *end = start + bufSize;
		uint16 len;

		while (start + sizeof(len) < end) {
			len = ntohs(*(uint16 *) start);
			if (end - start - sizeof(len) < len) {
				// We have not yet received a complete packet
				break;
			}

			start += sizeof(len);
			if (len >= sizeof(UDP_SRV_HDR)) {
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
