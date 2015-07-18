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

#include "tcppacket.h"
#include "icqsocket.h"


TcpOutPacket::TcpOutPacket()
{
	cursor += sizeof(uint16);
}

int TcpOutPacket::send(int sock)
{
	int n = cursor - data;
	*(uint16 *) data = htons(n - sizeof(uint16));

	return ::send(sock, (const char *) data, n, 0);
}


TcpInPacket::TcpInPacket(char *d, int n)
: IcqInPacket(d, n)
{
	*this >> header.ver >> header.reserved;
	*this >> header.cmd >> header.sessionCount;
}

