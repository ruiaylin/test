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

#ifndef _TCP_PACKET_H
#define _TCP_PACKET_H

#include <cstdio>
#include <string>
#include "icqpacket.h"
#include "icqsocket.h"

#pragma pack(1)
struct TCP_HEADER {
	uint16 ver;
	uint32 reserved;
	uint16 cmd;
	uint32 uin;
	uint8 face;
};
#pragma pack()

class TcpOutPacket : public IcqOutPacket {
public:
	TcpOutPacket() {
		cursor += sizeof(uint16);
	}
	int send(int sock);
};

class TcpInPacket : public IcqInPacket {
public:
	TcpInPacket(const char *d, int len);

	virtual uint16 getCmd() {
		return header.cmd;
	}

	TCP_HEADER header;
};


#endif
