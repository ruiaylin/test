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

#ifndef _PACKET_H
#define _PACKET_H

#include "icqtypes.h"

class Packet {
public:
	virtual const char *getData() = 0;
	virtual int getSize() = 0;
};

class OutPacket : public Packet {
public:
	virtual OutPacket &operator <<(uint8 b) = 0;
	virtual OutPacket &operator <<(uint16 w) = 0;
	virtual OutPacket &operator <<(uint32 dw) = 0;
	virtual OutPacket &operator <<(const char *str) = 0;
	virtual OutPacket &operator <<(string &s) = 0;
	virtual void writeData(const char *buf, int n) = 0;
};

class InPacket : public Packet {
public:
	virtual InPacket &operator >>(uint8 &b) = 0;
	virtual InPacket &operator >>(uint16 &w) = 0;
	virtual InPacket &operator >>(uint32 &dw) = 0;
	virtual InPacket &operator >>(const char *&str) = 0;
	virtual InPacket &operator >>(string &s) = 0;
	virtual uint16 getCmd() = 0;
	virtual const char *readData(int &n) = 0;
};

#endif
