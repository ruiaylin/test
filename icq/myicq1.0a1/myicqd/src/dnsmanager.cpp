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

#include "dnsmanager.h"
#include "server.h"
#include "queue.h"
#include "myicq.h"
#include "log.h"


#define IP_HASH_SIZE		64
#define IP_CACHE_TIMEOUT	(3 * 60 * 60)
#define MAX_IP_HASH_SIZE	256


ListHead IPCache::hash[IP_HASH_SIZE];

static Queue reqQueue;
static Queue resQueue;
static ListHead ipEntryList;	// LRU list
static int numEntries;

IMPLEMENT_SLAB(IPEntry, 128)


static int hashString(const char *key)
{
	int h = 0;
	while (*key)
		h = (h << 5) + h + *key++;
	return (h & (IP_HASH_SIZE - 1));
}

static int freeEntry(int n)
{
	int count = n;

	while (n > 0 && !ipEntryList.isEmpty()) {
		ListHead *pos = ipEntryList.removeHead();
		IPEntry *e = LIST_ENTRY(pos, IPEntry, listItem);

		e->hashItem.remove();
		e->release();

		n--;
	}

	count -= n;
	numEntries -= count;

	return count;
}


IPEntry::IPEntry(const char *n, Server *s)
{
	name[MAX_DOMAIN_NAME] = '\0';
	strncpy(name, n, MAX_DOMAIN_NAME);

	server = s;
	ip = INADDR_NONE;
	expire = curTime + IP_CACHE_TIMEOUT;
}

IPEntry *IPCache::get(const char *name)
{
	int i = hashString(name);
	ListHead *head = &hash[i];
	ListHead *pos;

	LIST_FOR_EACH(pos, head) {
		IPEntry *e = LIST_ENTRY(pos, IPEntry, hashItem);
		if (!strcmp(e->name, name)) {
			e->listItem.remove();

			if (e->expire <= curTime) {
				LOG(4) ("IPEntry (%s) expires.\n", e->name);

				e->hashItem.remove();
				e->release();
				break;
			}

			ipEntryList.add(&e->listItem);
			return e;
		}
	}
	return NULL;
}

void IPCache::put(IPEntry *e)
{
	int i = hashString(e->name);
	hash[i].add(&e->hashItem);

	if (++numEntries > MAX_IP_HASH_SIZE)
		freeEntry(MAX_IP_HASH_SIZE >> 4);
}

void DNSManager::resolve(const char *name, Server *s)
{
	IPEntry *e = new IPEntry(name, s);
	s->addRef();
	IPCache::put(e);

	e->addRef();
	reqQueue.put(&e->listItem);
}

void DNSManager::process()
{
	for (;;) {
		IPEntry *e = LIST_ENTRY(reqQueue.get(), IPEntry, listItem);
		hostent *hent = gethostbyname(e->name);
		if (hent)
			e->ip = *(uint32 *) hent->h_addr;

		resQueue.put(&e->listItem);
	}
}

void DNSManager::dispatch()
{
	while (!resQueue.isEmpty()) {
		IPEntry *e = LIST_ENTRY(resQueue.get(), IPEntry, listItem);
		Server *s = e->server;
		e->server = NULL;

		ipEntryList.add(&e->listItem);

		s->onDnsResolved(e->ip);
		s->release();
		e->release();
	}
}
