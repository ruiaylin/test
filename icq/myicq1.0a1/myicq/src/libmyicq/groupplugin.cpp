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
#include <map>

typedef map<string, GroupPlugin *> PLUGIN_MAP;

static PLUGIN_MAP pluginMap;


GroupSession *GroupPlugin::createSession(IcqGroup *g)
{
	if (!info.createGroup)
		return NULL;
	
	if (info.numMembers && g->numMembers < info.numMembers)
		return NULL;

	return info.createGroup(g);
}

GroupPlugin *GroupPlugin::get(const char *name)
{
	GroupPlugin *p;

	PLUGIN_MAP::iterator it = pluginMap.find(name);
	if (it != pluginMap.end())
		p = (*it).second;
	else {
		if (!name || !*name)
			p = new GroupPlugin;
		else
			p = load(name);
		if (p)
			pluginMap[name] = p;
	}
	return p;
}
