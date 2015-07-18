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

#include "icqpacket.h"
#include "list.h"
#include "slab.h"
#include <time.h>

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

class UdpSession;

class UdpOutPacket : public IcqOutPacket {
public:
	UdpOutPacket(UdpSession *s = NULL);

	int send(int sock, uint32 ip, uint16 port);

	ListHead sendItem;
	ListHead globalSendItem;

	int attempts;
	time_t expire;
	uint16 seq;
	UdpSession *session;

	DECLARE_SLAB(UdpOutPacket)
};

class UdpInPacket : public IcqInPacket {
public:
	UdpInPacket(char *d, int n);

	bool decrypt(char *subkey);

	UDP_CLI_HDR header;
};

#endif
