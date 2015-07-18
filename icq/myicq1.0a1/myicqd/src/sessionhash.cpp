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

#include <stdlib.h>
#include "sessionhash.h"
#include "udpsession.h"
#include "icqpacket.h"

#define HASH_SIZE		(1 << 16)
#define HASH_MASK		(HASH_SIZE - 1)

struct HASH {
	ListHead ipport;
	ListHead uin;
};

static HASH bucket[HASH_SIZE];


inline int hashfn(uint32 uin)
{
	uin ^= (uin >> 16);
	uin ^= (uin >> 8);
	return (uin & HASH_MASK);
}

inline int hashfn(uint32 ip, uint16 port)
{
	int h = (ip ^ port);
	h ^= (h >> 16);
	h ^= (h >> 8);
	return (h & HASH_MASK);
}

inline uint32 rand32()
{
	return ((rand() << 15) | (rand() & 0x7fff));
}

UdpSession *SessionHash::get(uint32 uin)
{
	ListHead *pos;
	ListHead *head = &bucket[hashfn(uin)].uin;

	LIST_FOR_EACH(pos, head) {
		UdpSession *p = LIST_ENTRY(pos, UdpSession, uinItem);
		if (p->uin == uin)
			return p;
	}
	return NULL;
}

UdpSession *SessionHash::get(uint32 ip, uint16 port)
{
	ListHead *pos;
	ListHead *head = &bucket[hashfn(ip, port)].ipport;

	LIST_FOR_EACH(pos, head) {
		UdpSession *p = LIST_ENTRY(pos, UdpSession, ipportItem);
		if (p->ip == ip && p->port == port)
			return p;
	}
	return NULL;
}

void SessionHash::put(UdpSession *p, uint32 ip, uint16 port)
{
	int i = hashfn(ip, port);
	bucket[i].ipport.addHead(&p->ipportItem);	
}

void SessionHash::put(UdpSession *p, uint32 uin)
{
	int i = hashfn(uin);
	bucket[i].uin.addHead(&p->uinItem);
}

void SessionHash::random(IcqOutPacket &out, int n)
{
	uint16 num = 0;
	uint8 *old = out.skip(sizeof(num));

	int start = rand32() & HASH_MASK;
	int i = start;
	do {
		i = ((i + 1) & HASH_MASK);

		ListHead *pos, *head = &bucket[i].ipport;
		LIST_FOR_EACH(pos, head) {
			UdpSession *s = LIST_ENTRY(pos, UdpSession, ipportItem);
			if (s->status != STATUS_INVIS && s->status != STATUS_OFFLINE) {
				out << s->uin;
				out << (uint8) 1;		// online
				out << s->face;
				out << s->nickname;
				out << s->province;

				if (++num >= n)
					break;
			}
		}

	} while (num < n && i != start);

	old = out.setCursor(old);
	out << num;
	out.setCursor(old);
}
