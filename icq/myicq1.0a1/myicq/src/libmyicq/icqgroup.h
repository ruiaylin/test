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

#ifndef _ICQ_GROUP_H
#define _ICQ_GROUP_H

#include "group.h"

#define MAX_GROUP_MEMBERS	8

enum {
	GROUP_OPTION_MSG_NOTIFY,
	GROUP_OPTION_MSG_POPUP,
	GROUP_OPTION_MSG_IGNORE,
};

class GroupPlugin;
class GroupSession;
class InPacket;

class IcqGroup : public Group {
public:
	IcqGroup(GroupPlugin *p, uint32 id);
	~IcqGroup();

	void *getIcon();

	virtual IcqLinkBase *getLink();
	virtual void destroy();
	virtual int getMyPos();
	virtual GroupMember *getMemberInfo(int i);
	virtual OutPacket *createPacket(uint16 cmd);
	virtual void sendPacket(OutPacket *out);
	virtual void start();

	void addMember(uint32 uin, uint8 face, const char *nick);
	void removeMember(uint32 uin);
	void destroyAll();
	GroupMember *getMemberInfo(uint32 uin) {
		return getMemberInfo(getPos(uin));
	}

	void onGroupCmd(InPacket &in);
	void onStart();

	GroupPlugin *plugin;
	GroupSession *session;

	uint32 id;
	string name;
	int numMembers;
	GroupMember *members[MAX_GROUP_MEMBERS];
	int msgOption;

private:
	int getMaxMembers();
	int getPos(uint32 uin);
};


#endif
