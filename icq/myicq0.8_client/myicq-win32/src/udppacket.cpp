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
#include "ndes.h"


UdpOutPacket::UdpOutPacket()
{
	realData = data;
	attempts = 0;
	expire = 0;
	cmd = 0;
	seq = 0;
}

void UdpOutPacket::encrypt()
{
	char *p = realData;
	int n = cursor - p;

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