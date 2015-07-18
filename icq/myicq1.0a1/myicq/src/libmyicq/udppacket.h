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

#ifndef _UDP_PACKET_H
#define _UDP_PACKET_H

#include <time.h>
#include "icqpacket.h"

#pragma pack(1)

struct UDP_CLI_HDR {
	uint16 ver;
	uint32 reserved;
	uint32 uin;
	uint32 sid;
	uint16 cmd;
	uint16 seq;
	uint16 cc;		// check code
};

struct UDP_SRV_HDR {
	uint16 ver;
	uint32 reserved;
	uint32 uin;
	uint32 sid;
	uint16 cmd;
	uint16 seq;
	uint16 ackseq;
};

#pragma pack()


class UdpOutPacket : public IcqOutPacket {
public:
	UdpOutPacket();

	void encrypt();

	int attempts;
	time_t expire;
	uint16 cmd;
	uint16 seq;
};

class UdpInPacket : public IcqInPacket {
public:
	UdpInPacket(const char *d, int len);

	virtual uint16 getCmd() {
		return header.cmd;
	}

	UDP_SRV_HDR header;
};

#endif
