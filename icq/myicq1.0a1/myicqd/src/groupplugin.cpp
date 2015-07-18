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

#include "groupplugin.h"
#include "icqgroup.h"
#include <string.h>

#define MAX_PLUGINS			32
#define GROUP_HASH_SIZE		1024
#define MIN_GROUP_ID		10000
#define MAX_GROUP_ID		0x0fffffff
#define MAX_GROUPS			25

int GroupPlugin::numPlugins;
GroupPlugin *GroupPlugin::plugins[MAX_PLUGINS];


static ListHead groupHash[GROUP_HASH_SIZE];


inline int hash(uint32 key)
{
	return (key & (GROUP_HASH_SIZE - 1));
}

inline int produceGroupID()
{
	static int lastGroupNumber = MIN_GROUP_ID;

	if (++lastGroupNumber > MAX_GROUP_ID)
		lastGroupNumber = MIN_GROUP_ID;
	return lastGroupNumber;
}

bool GroupPlugin::registerPlugin(GroupPlugin *p)
{
	if (numPlugins >= MAX_PLUGINS)
		return false;

	p->type = numPlugins;
	plugins[numPlugins++] = p;
	return true;
}

IcqGroup *GroupPlugin::getGroup(uint32 id)
{
	ListHead *head = &groupHash[hash(id)];
	ListHead *pos;

	LIST_FOR_EACH(pos, head) {
		IcqGroup *g = LIST_ENTRY(pos, IcqGroup, hashItem);
		if (g->id == id)
			return g;
	}

	return NULL;
}

IcqGroup *GroupPlugin::createGroup(int type)
{
	if (type < 0 || type >= numPlugins)
		return NULL;

	return plugins[type]->createGroup();
}

GroupPlugin::GroupPlugin()
{
	numGroups = 0;
}

IcqGroup *GroupPlugin::createGroup()
{
	if (numGroups >= MAX_GROUPS)
		return NULL;

	uint32 id = produceGroupID();
	IcqGroup *g = new IcqGroup(this, id);

	groupHash[hash(id)].add(&g->hashItem);
	groupList.add(&g->listItem);

	numGroups++;
	return g;
}

void GroupPlugin::destroyGroup(IcqGroup *g)
{
	g->hashItem.remove();
	g->listItem.remove();
	delete g;
	
	numGroups--;
}

GroupSession *GroupPlugin::createSession(IcqGroup *g, int n)
{
	if (!info.createGroup)
		return NULL;

	if (info.maxMembers && n != info.maxMembers)
		return NULL;

	return info.createGroup(g);
}
