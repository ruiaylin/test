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

#include <string.h>
#include "udppacket.h"
#include "ndes.h"


UdpOutPacket::UdpOutPacket(UdpSession *p)
{
	attempts = 0;
	expire = 0;
	session = p;
}

void UdpOutPacket::write8(uint8 b)
{
	if (cursor <= data + MAX_PACKET_SIZE - sizeof(b)) {
		*(uint8 *) cursor = b;
		cursor += sizeof(b);
	}
}

void UdpOutPacket::write16(uint16 w)
{
	if (cursor <= data + MAX_PACKET_SIZE - sizeof(w)) {
		*(uint16 *) cursor = htons(w);
		cursor += sizeof(w);
	}
}

void UdpOutPacket::write32(uint32 dw)
{
	if (cursor <= data + MAX_PACKET_SIZE - sizeof(dw)) {
		*(uint32 *) cursor = htonl(dw);
		cursor += sizeof(dw);
	}
}

void UdpOutPacket::writeString(const char *str)
{
	uint16 len = strlen(str) + 1;
	if (cursor <= data + MAX_PACKET_SIZE - sizeof(len) - len) {
		write16(len);
		strcpy(cursor, str);
		cursor += len;
	}
}

int UdpOutPacket::send(int sock, uint32 ip, uint16 port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	return sendto(sock, data, cursor - data, 0, (sockaddr *) &addr, sizeof(addr));
}


uint8 UdpInPacket::read8()
{
	uint8 b = 0;
	if (cursor <= data + datalen  - sizeof(uint8)) {
		b = *cursor;
		cursor += sizeof(b);
	}
	return b;
}

uint16 UdpInPacket::read16()
{
	uint16 w = 0;
	if (cursor <= data + datalen - sizeof(w)) {
		w = ntohs(*(uint16 *) cursor);
		cursor += sizeof(w);
	}
	return w;
}

uint32 UdpInPacket::read32()
{
	uint32 dw = 0;
	if (cursor <= data + datalen - sizeof(dw)) {
		dw = ntohl(*(uint32 *) cursor);
		cursor += sizeof(uint32);
	}
	return dw;
}

const char *UdpInPacket::readString()
{
	const char *str = "";
	uint16 len = read16();
	if (cursor <= data + datalen - len && !cursor[len - 1]) {
		str = cursor;
		cursor += len;
	}
	return str;
}

int UdpInPacket::recv(int sock)
{
	socklen_t len = sizeof(addr);
	datalen = recvfrom(sock, data, MAX_PACKET_SIZE, 0, (sockaddr *) &addr, &len);
	if (datalen > 0)
		cursor = data + sizeof(UDP_HEADER);
	return datalen;
}

void UdpInPacket::decrypt(const char *key)
{
	int n = datalen - sizeof(UDP_HEADER);
	if (n > 0) {
		setkey((char *) key);
	
		char *p = data + sizeof(UDP_HEADER);
		while (n > 0) {
			dedes(p);
			p += 8;
			n -= 8;
		}
	}
}
