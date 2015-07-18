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

#include "icqpacket.h"
#include "icqsocket.h"


void IcqOutPacket::write16(uint16 w)
{
	if (cursor - data + sizeof(w) <= UDP_PACKET_SIZE) {
		*(uint16 *) cursor = w;
		cursor += sizeof(w);
	}
}

void IcqOutPacket::write32(uint32 dw)
{
	if (cursor - data + sizeof(dw) <= UDP_PACKET_SIZE) {
		*(uint32 *) cursor = dw;
		cursor += sizeof(dw);
	}
}

void IcqOutPacket::writeData(uint8 *buf, int n)
{
	if (cursor - data + n <= UDP_PACKET_SIZE) {
		memcpy(cursor, buf, n);
		cursor += n;
	}
}

void IcqOutPacket::writeString(const char *str, int n)
{
	uint16 len = n + 1;
	if (cursor + len <= data + UDP_PACKET_SIZE - sizeof(len)) {
		*this << len;
		memcpy(cursor, str, len);
		cursor += len;
	}
}

OutPacket &IcqOutPacket::operator <<(uint8 b)
{
	if (cursor <= data + UDP_PACKET_SIZE - sizeof(b)) {
		*(uint8 *) cursor = b;
		cursor += sizeof(b);
	}
	return *this;
}

OutPacket &IcqOutPacket::operator <<(uint16 w)
{
	write16(htons(w));
	return *this;
}

OutPacket &IcqOutPacket::operator <<(uint32 dw)
{
	write32(htonl(dw));
	return *this;
}

OutPacket &IcqOutPacket::operator <<(ICQ_STR &str)
{
	writeString(str.text, str.len);
	return *this;
}


uint16 IcqInPacket::read16()
{
	uint16 w = 0;
	if (cursor <= data + datalen - sizeof(w)) {
		w = *(uint16 *) cursor;
		cursor += sizeof(w);
	}
	return w;
}

uint32 IcqInPacket::read32()
{
	uint32 dw = 0;
	if (cursor <= data + datalen - sizeof(dw)) {
		dw = *(uint32 *) cursor;
		cursor += sizeof(dw);
	}
	return dw;
}

uint8 *IcqInPacket::readData(int &n)
{
	n = datalen - (cursor - data);
	return cursor;
}

InPacket &IcqInPacket::operator >>(uint8 &b)
{
	b = 0;
	if (cursor <= data + datalen - sizeof(b))
		b = *cursor++;
	return *this;
}

InPacket &IcqInPacket::operator >>(uint16 &w)
{
	w = ntohs(read16());
	return *this;
}

InPacket &IcqInPacket::operator >>(uint32 &dw)
{
	dw = ntohl(read32());
	return *this;
}

InPacket &IcqInPacket::operator >>(ICQ_STR &str)
{
	uint16 len;
	operator >>(len);

	if (len && cursor <= data + datalen - len && !cursor[len - 1]) {
		str.text = (char *) cursor;
		str.len = len - 1;
		cursor += len;
	} else {
		str.text = "";
		str.len = 0;
	}
	return *this;
}
