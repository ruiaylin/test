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

typedef int		socklen_t;

#define close	closesocket
#define ioctl	ioctlsocket

#define EINPROGRESS		WSAEWOULDBLOCK

inline int getSocketError()
{
	return WSAGetLastError();
}

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>

inline int getSocketError()
{
	return errno;
}

#endif

#endif
