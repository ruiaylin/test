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


UdpInPacket::UdpInPacket(const char *d, int len)
: IcqInPacket(d, len)
{
	*this >> header.ver >> header.reserved;
	*this >> header.uin >> header.sid >> header.cmd;
	*this >> header.seq >> header.ackseq;
}


UdpOutPacket::UdpOutPacket()
{
	attempts = 0;
	expire = 0;
	cmd = 0;
	seq = 0;
}

void UdpOutPacket::encrypt()
{
	char *p = data + sizeof(UDP_CLI_HDR);
	int n = cursor - p;
	if (n <= 0)
		return;

	// Check code
	uint8 i = (rand() & 0xff) % n;
	uint8 v = p[i];
	uint16 cc = ((i << 8) | v);
	((UDP_CLI_HDR *) data)->cc = htons(cc);

	while (n > 0) {
		if (n < 8) {
			int d = 8 - n;
			memset(p + n, 0, d);
			cursor += d;
		}

		endes(p);

		p += 8;
		n -= 8;
	}
}
