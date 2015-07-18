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

#ifndef _DNS_MANAGER_H
#define _DNS_MANAGER_H

#include "refobject.h"
#include "list.h"
#include "slab.h"
#include <time.h>

#define MAX_DOMAIN_NAME		64

class Server;

class IPEntry : public RefObject {
public:
	IPEntry(const char *name, Server *s);

	char name[MAX_DOMAIN_NAME + 1];
	uint32 ip;
	time_t expire;
	Server *server;

	ListHead hashItem;
	ListHead listItem;

	DECLARE_SLAB(IPEntry)
};

class IPCache {
public:
	static IPEntry *get(const char *name);
	static void put(IPEntry *e);

private:
	static ListHead hash[];
};


class DNSManager {
public:
	static void resolve(const char *name, Server *s);
	static void process();
	static void dispatch();
};


#endif
