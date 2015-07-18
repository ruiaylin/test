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

#ifndef _GROUP_PLUGIN_H
#define _GROUP_PLUGIN_H

#include "list.h"
#include "groupsession.h"
#include "icqgroup.h"
#include <string>

using namespace std;

typedef void (*GET_PLUGIN_INFO)(PluginInfo *);


class GroupPlugin {
public:
	static bool registerPlugin(GroupPlugin *p);
	static IcqGroup *createGroup(int type);
	static IcqGroup *getGroup(uint32 id);
	static GroupPlugin *getPlugin(int type) {
		if (type >= 0 && type < numPlugins)
			return plugins[type];
		return NULL;
	}

	GroupPlugin();

	int getMaxMembers() {
		return  (info.maxMembers ? info.maxMembers : MAX_GROUP_MEMBERS);
	}
	IcqGroup *createGroup();
	void destroyGroup(IcqGroup *g);
	GroupSession *createSession(IcqGroup *g, int n);

	PluginInfo info;
	string name, displayName;
	int numGroups;
	int type;
	ListHead groupList;
	
	static GroupPlugin *plugins[];
	static int numPlugins;
};


#endif
