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

#ifndef _GROUP_SESSION_H
#define _GROUP_SESSION_H

#include "icqtypes.h"

class Group;
class InPacket;

class GroupSession {
public:
	virtual void destroy() = 0;

	virtual void onMemberEntered(int i) = 0;
	virtual void onMemberExited(int i) = 0;
	virtual void onStart() = 0;

	virtual void onPacketReceived(InPacket &in) = 0;
};

typedef GroupSession *(*CREATE_GROUP)(Group *);


class GroupPluginInfo {
public:
	GroupPluginInfo() {
		icon = 0;
		numMembers = 0;
		createGroup = 0;
	}

	void *icon;
	int numMembers;
	CREATE_GROUP createGroup;
};


#endif
