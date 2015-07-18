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

#include "icqtypes.h"
#include "groupsession.h"

typedef void (*GET_PLUGIN_INFO)(GroupPluginInfo *);


class IcqGroup;

class GroupPlugin {
public:
	GroupSession *createSession(IcqGroup *g);

	static GroupPlugin *get(const char *name);

	GroupPluginInfo info;

private:
	static GroupPlugin *load(const char *name);
};


#endif
