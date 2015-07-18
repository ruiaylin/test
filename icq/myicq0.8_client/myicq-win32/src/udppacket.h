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
#include "icqsocket.h"

#pragma pack(1)
struct UDP_HEADER {
	uint16 ver;
	uint32 reserved;
	uint32 sid;
	uint16 seq;
	uint16 ackseq;
	uint16 cmd;
	uint32 uin;
};
#pragma pack()


class UdpOutPacket : public IcqOutPacket {
public:
	UdpOutPacket();

	void beginData() {
		realData = cursor;
	}
	void encrypt();

	int attempts;
	time_t expire;
	uint16 cmd;
	uint16 seq;

private:
	char *realData;
};

class UdpInPacket : public IcqInPacket {
public:
	UdpInPacket(const char *d, int len)
	: IcqInPacket(d, len) {
		cursor = data + sizeof(UDP_HEADER);
	}

	uint16 getVersion() { return ntohs(((UDP_HEADER *) data)->ver); }
	uint32 getSID() { return ntohl(((UDP_HEADER *) data)->sid); }
	uint16 getSeq() { return ntohs(((UDP_HEADER *) data)->seq); }
	uint16 getAckSeq() { return ntohs(((UDP_HEADER *) data)->ackseq); }
	uint16 getCmd() { return ntohs(((UDP_HEADER *) data)->cmd); }
	uint32 getUIN() { return ntohl(((UDP_HEADER *) data)->uin); }
};

#endif
