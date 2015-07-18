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

#ifndef _ICQ_PACKET_H
#define _ICQ_PACKET_H

#include "packet.h"
#include <string.h>


#define UDP_PACKET_SIZE	1024


class IcqOutPacket : public OutPacket {
public:
	IcqOutPacket() {
		reset();
	}

	virtual OutPacket &operator <<(uint8 b);
	virtual OutPacket &operator <<(uint16 w);
	virtual OutPacket &operator <<(uint32 dw);
	virtual OutPacket &operator <<(ICQ_STR &str);

	void reset() {
		cursor = data;
	}
	uint8 *getData() {
		return data;
	}
	int getLength() {
		return cursor - data;
	}

	void write16(uint16 w);
	void write32(uint32 dw);
	void writeData(uint8 *buf, int n);

	OutPacket &operator <<(const char *str) {
		writeString(str, strlen(str));
		return *this;
	}
	void writeString(const char *str, int n);

	uint8 *skip(int delta) {
		cursor += delta;
		return (cursor - delta);
	}
	uint8 *setCursor(uint8 *cur) {
		uint8 *old = cursor;
		cursor = cur;
		return old;
	}

protected:
	uint8 data[UDP_PACKET_SIZE];
	uint8 *cursor;
};


class IcqInPacket : public InPacket {
public:
	IcqInPacket(char *d, int n) {
		cursor = data = (uint8 *) d;
		datalen = n;
	}

	virtual InPacket &operator >>(uint8 &b);
	virtual InPacket &operator >>(uint16 &w);
	virtual InPacket &operator >>(uint32 &dw);
	virtual InPacket &operator >>(ICQ_STR &str);

	uint32 read32();
	uint16 read16();
	uint8 *readData(int &n);

protected:
	uint8 *data;
	uint8 *cursor;
	int datalen;
};

#endif
