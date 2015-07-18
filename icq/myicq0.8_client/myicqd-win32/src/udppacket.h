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
#include "icq.h"
#include "icqlist.h"
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

#define MAX_PACKET_SIZE	1024


class IcqPacket {
public:
	IcqPacket() {
		cursor = data;
	}

protected:
	char data[MAX_PACKET_SIZE];
	char *cursor;
};


class UdpOutPacket : public IcqPacket {
friend class UdpSession;

public:
	UdpOutPacket(UdpSession *);

	void write8(uint8 b);
	void write16(uint16 w);
	void write32(uint32 dw);
	void writeString(const char *str);

	char *skip(int delta) {
		cursor += delta;
		return (cursor - delta);
	}
	char *setCursor(char *cur) {
		char *old = cursor;
		cursor = cur;
		return old;
	}
	uint16 getSeq() { return ntohs(((UDP_HEADER *) data)->seq); }
	int send(int sock, uint32 ip, uint16 port);

	IcqListItem sendItem;
	IcqListItem globalSendItem;

private:
	int attempts;
	time_t expire;
	UdpSession *session;
};

class UdpInPacket : public IcqPacket {
public:
	UdpInPacket() {
		datalen = 0;
	}

	uint8 read8();
	uint16 read16();
	uint32 read32();
	const char *readString();
	
	uint16 getVersion() { return ntohs(((UDP_HEADER *) data)->ver); }
	uint32 getSID() { return ntohl(((UDP_HEADER *) data)->sid); }
	uint16 getSeq() { return ntohs(((UDP_HEADER *) data)->seq); }
	uint16 getCmd() { return ntohs(((UDP_HEADER *) data)->cmd); }
	uint32 getUIN() { return ntohl(((UDP_HEADER *) data)->uin); }

	uint32 getIP() {
		return ntohl(addr.sin_addr.s_addr);
	}
	uint16 getPort() {
		return ntohs(addr.sin_port);
	}

	void decrypt(const char *key);
	int recv(int sock);

private:
	int datalen;
	sockaddr_in addr;
};

#endif
