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

#ifndef _ICQ_SOCKET_H
#define _ICQ_SOCKET_H

#ifdef _WIN32

#include <winsock.h>

#define close			closesocket
#define EINPROGRESS		WSAEWOULDBLOCK
typedef int				socklen_t;

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>

#endif

class SocketListener;

class IcqSocket {
public:
	static void destroy();

	static int createSocket(int type, SocketListener *l);
	static int createListenSocket(const char *name);
	static int acceptSocket(int listenSock, SocketListener *l);
	static void closeSocket(int sock);
	static void enableWrite(int sock);
	static int getSocketError();
};

#endif
