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
#include "udpsession.h"
#include "sessionhash.h"

IcqList SessionHash::alive[ALIVE_HASH_SIZE];
IcqList SessionHash::dead[DEAD_HASH_SIZE];


UdpSession *SessionHash::getAlive(uint32 uin)
{
	UdpSession *p;
	IcqListItem *pos;
	IcqListItem *head = &alive[hashfn(uin)].head;

	LIST_FOR_EACH(pos, head) {
		p = LIST_ENTRY(pos, UdpSession, listItem);
		if (p->uin == uin)
			return p;
	}
	return NULL;
}


UdpSession *SessionHash::getDead(uint32 ip, uint16 port)
{
	UdpSession *p;
	IcqListItem *pos;
	IcqListItem *head = &dead[hashfn(ip, port)].head;

	LIST_FOR_EACH(pos, head) {
		p = LIST_ENTRY(pos, UdpSession, listItem);
		if (p->ip == ip && p->port == port)
			return p;
	}
	return NULL;
}

int SessionHash::random(uint32 results[], int n)
{
	int start = rand() % ALIVE_HASH_SIZE;
	int num = 0;
	for (int i = start + 1; num < n && i != start; i++) {
		if (i >= ALIVE_HASH_SIZE)
			i = 0;

		IcqListItem *pos, *head = &alive[i].head;
		LIST_FOR_EACH(pos, head) {
			UdpSession *p = LIST_ENTRY(pos, UdpSession, listItem);
			results[num++] = p->uin;
			if (num >= n)
				break;
		}
	}
	return num;
}
