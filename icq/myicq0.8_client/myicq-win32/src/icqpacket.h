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

#define MAX_PACKET_SIZE	4096


class IcqOutPacket : public OutPacket {
public:
	IcqOutPacket() {
		cursor = data;
	}
	const char *getData() {
		return data;
	}
	int getSize() {
		return (cursor - data);
	}

	int setCursor(int off);
	OutPacket &operator <<(string &str) {
		return operator <<(str.c_str());
	}

	virtual OutPacket &operator <<(uint8 b);
	virtual OutPacket &operator <<(uint16 w);
	virtual OutPacket &operator <<(uint32 dw);
	virtual OutPacket &operator <<(const char *str);
	virtual void writeData(const char *buf, int n);
	
protected:
	char data[MAX_PACKET_SIZE];
	char *cursor;
};


class IcqInPacket : public InPacket {
public:
	IcqInPacket(const char *d, int len) {
		cursor = data = d;
		datalen = len;
	}

	virtual const char *getData() {
		return data;
	}
	virtual int getSize() {
		return datalen;
	}
	virtual InPacket &operator >>(uint8 &b);
	virtual InPacket &operator >>(uint16 &w);
	virtual InPacket &operator >>(uint32 &dw);
	virtual InPacket &operator >>(const char *&str);
	InPacket &operator >>(string &str);
	const char *readData(int &n);

protected:
	const char *data;
	int datalen;
	const char *cursor;
};


#endif
