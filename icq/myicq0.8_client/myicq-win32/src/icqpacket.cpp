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

#include "icqpacket.h"
#include "icqsocket.h"

int IcqOutPacket::setCursor(int pos) {
	int old = cursor - data;
	cursor = data + pos;
	return old;
}

OutPacket &IcqOutPacket::operator <<(uint8 b)
{
	if (cursor <= data + MAX_PACKET_SIZE - sizeof(b)) {
		*(uint8 *) cursor = b;
		cursor += sizeof(b);
	}
	return (*this);
}

OutPacket &IcqOutPacket::operator <<(uint16 w)
{
	if (cursor <= data + MAX_PACKET_SIZE - sizeof(w)) {
		*(uint16 *) cursor = htons(w);
		cursor += sizeof(w);
	}
	return (*this);
}

OutPacket &IcqOutPacket::operator <<(uint32 dw)
{
	if (cursor <= data + MAX_PACKET_SIZE - sizeof(dw)) {
		*(uint32 *) cursor = htonl(dw);
		cursor += sizeof(dw);
	}
	return (*this);
}

OutPacket &IcqOutPacket::operator <<(const char *str)
{
	uint16 len = strlen(str) + 1;
	if (cursor <= data + MAX_PACKET_SIZE - sizeof(len) - len) {
		operator <<(len);
		strcpy(cursor, str);
		cursor += len;
	}
	return (*this);
}

void IcqOutPacket::writeData(const char *buf, int n)
{
	if (cursor <= data + MAX_PACKET_SIZE - n) {
		memcpy(cursor, buf, n);
		cursor += n;
	}
}

InPacket &IcqInPacket::operator >>(uint8 &b)
{
	if (cursor <= data + datalen - sizeof(b)) {
		b = *(uint8 *) cursor;
		cursor += sizeof(b);
	} else
		b = 0;
	return (*this);
}

InPacket &IcqInPacket::operator >>(uint16 &w)
{
	if (cursor <= data + datalen - sizeof(w)) {
		w = ntohs(*(uint16 *) cursor);
		cursor += sizeof(w);
	} else
		w = 0;
	return (*this);
}

InPacket &IcqInPacket::operator >>(uint32 &dw)
{
	if (cursor <= data + datalen - sizeof(dw)) {
		dw = ntohl(*(uint32 *) cursor);
		cursor += sizeof(dw);
	} else
		dw = 0;
	return (*this);

}

InPacket &IcqInPacket::operator >>(const char *&str)
{
	uint16 len;
	operator >>(len);
	
	if (cursor <= data + datalen - len && !cursor[len - 1]) {
		str = cursor;
		cursor += len;
	} else
		str = "";
	return (*this);
}

InPacket &IcqInPacket::operator >>(string &str)
{
	const char *p;
	operator >>(p);
	str = p;
	return (*this);
}

const char *IcqInPacket::readData(int &n)
{
	n = datalen - (cursor - data);
	return cursor;
}
