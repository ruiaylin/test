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

#ifndef _SESSION_HASH_H_
#define _SESSION_HASH_H_

#include "udpsession.h"

#define ALIVE_HASH_SIZE	(1024 * 1024)
#define DEAD_HASH_SIZE	128

class SessionHash {
public:
	static UdpSession *getAlive(uint32 uin);
	static UdpSession *getDead(uint32 ip, uint16 port);
	static void addAlive(UdpSession *p) {
		alive[hashfn(p->uin)].addHead(&p->listItem);
	}
	static void addDead(UdpSession *p) {
		dead[hashfn(p->ip, p->port)].add(&p->listItem);
	}
	static int random(uint32 results[], int n);

private:
	static int hashfn(uint32 uin) {
		uin ^= (uin >> 16);
		uin ^= (uin >> 8);
		return (uin & (ALIVE_HASH_SIZE - 1));
	}

	static int hashfn(uint32 ip, uint16 port) {
		int h = (ip ^ port);
		h ^= (h >> 16);
		h ^= (h >> 8);
		return (h & (DEAD_HASH_SIZE - 1));
	}

	static IcqList alive[ALIVE_HASH_SIZE];
	static IcqList dead[DEAD_HASH_SIZE];
};

#endif
