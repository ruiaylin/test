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

#include "udppacket.h"
#include "icqsocket.h"
#include "ndes.h"
#include <string.h>


IMPLEMENT_SLAB(UdpOutPacket, 128)


UdpOutPacket::UdpOutPacket(UdpSession *s)
{
	session = s;

	attempts = 0;
	expire = 0;
}

int UdpOutPacket::send(int sock, uint32 ip, uint16 port)
{
	sockaddr_in addr;
	// Is it neccesary?
	//memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = port;
	return sendto(sock, (char *) data, cursor - data, 0, (sockaddr *) &addr, sizeof(addr));
}


UdpInPacket::UdpInPacket(char *d, int n)
: IcqInPacket(d, n)
{
	*this >> header.ver >> header.reserved;
	*this >> header.uin >> header.sid >> header.cmd;
	*this >> header.seq >> header.cc;
}

bool UdpInPacket::decrypt(char *subkey)
{
	int len = datalen - sizeof(UDP_CLI_HDR);
	if (!len)
		return true;

	if (len < 0 || (len & 7))
		return false;

	uint8 *d = data + sizeof(UDP_CLI_HDR);
	uint8 *p = d;
	int n = len;
	while (n > 0) {
		dedes((char *) p, subkey);
		p += 8;
		n -= 8;
	}

	// check code
	uint8 i = ((header.cc >> 8) & 0xff);
	if (i >= len)
		return false;

	uint8 v = (header.cc & 0xff);
	return (d[i] == v);
}
