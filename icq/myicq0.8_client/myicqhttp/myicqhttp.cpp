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

#ifdef _WIN32
#include <windows.h>

#define close		closesocket
typedef int			socklen_t;

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#endif

#include <iostream.h>

#define MYICQ_HTTP_PORT		443
#define MYICQ_PORT			8000
#define MAX_PACKET_SIZE		4096

typedef unsigned short		uint16;
typedef unsigned int		uint32;


inline int getSocketError()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}


static sockaddr_in destAddr;


static int onHttpReceive(int httpSock, int udpSock, char *buf, int &bufSize)
{
	int n = recv(httpSock, buf + bufSize, MAX_PACKET_SIZE - bufSize, 0);
	if (n > 0) {
		bufSize += n;
		char *start = buf;
		char *end = start + bufSize;
		uint16 len;

		while (start + sizeof(len) < end) {
			len = ntohs(*(uint16 *) start);
			if (end - start - sizeof(len) < len)
				break;

			start += sizeof(len);
			send(udpSock, start, len, 0);
			start += len;
		}

		bufSize = end - start;
		if (bufSize > 0)
			memcpy(buf, start, bufSize);
	}

	return n;
}

static int onUdpReceive(int httpSock, int udpSock)
{
	char buf[MAX_PACKET_SIZE];

	int n = recv(udpSock, buf + sizeof(uint16), MAX_PACKET_SIZE - sizeof(uint16), 0);
	if (n > 0) {
		*(uint16 *) buf = htons(n);
		send(httpSock, buf, n + sizeof(uint16), 0);
	}

	return n;
}

static DWORD WINAPI processRequest(LPVOID param)
{
	int httpSock = (int) param;

	int udpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSock < 0) {
		close(httpSock);
		return 1;
	}

	connect(udpSock, (sockaddr *) &destAddr, sizeof(destAddr));

	char buf[MAX_PACKET_SIZE];
	int bufSize = 0;
	int nfds = (httpSock > udpSock ? httpSock : udpSock);
	fd_set rdset;

	while (true) {
		FD_ZERO(&rdset);
		FD_SET(httpSock, &rdset);
		FD_SET(udpSock, &rdset);

		if (select(nfds + 1, &rdset, NULL, NULL, NULL) < 0)
			break;

		if (FD_ISSET(httpSock, &rdset) &&
			onHttpReceive(httpSock, udpSock, buf, bufSize) <= 0)
			break;

		if (FD_ISSET(udpSock, &rdset) &&
			onUdpReceive(httpSock, udpSock) <= 0)
			break;
	}

	close(udpSock);
	close(httpSock);
	return 0;
}


int main(int argc, char *argv[])
{
#ifdef _WIN32
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsaData) != 0) {
		cerr << "WSAStartup() failed." << endl;
		return 1;
	}
#endif

	const char *host = "localhost";
	uint16 port = MYICQ_PORT;

	if (argc > 1)
		host = argv[1];
	if (argc > 2)
		port = atoi(argv[2]);

	memset(&destAddr, 0, sizeof(destAddr));
	destAddr.sin_family = AF_INET;
	if ((destAddr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		hostent *he = gethostbyname(host);
		if (!he) {
			cerr << "Can not resolve host: " << host << endl;
			return 1;
		}
		destAddr.sin_addr = *(in_addr *) he->h_addr;
	}
	destAddr.sin_port = htons(port);

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(MYICQ_HTTP_PORT);
	if (bind(listen_fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
		cerr << "Can not bind on port: " << MYICQ_HTTP_PORT << endl;
		close(listen_fd);
		return 1;
	}

	listen(listen_fd, 20);

	cout << "MyICQ http server is now started." << endl;

	while (true) {
		socklen_t addrlen = sizeof(addr);
		int new_fd = accept(listen_fd, (sockaddr *) &addr, &addrlen);
		if (new_fd >= 0) {
			DWORD id;
			CreateThread(NULL, 0, processRequest, (LPVOID) new_fd, 0, &id);
		}
	}

	close(listen_fd);

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
