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
#include "slab.h"

#define MAX_GROUP_NAME_LEN	16
#define MAX_GROUP_PASS_LEN	16
#define MAX_GROUP_MEMBERS	8

class UdpSession;
class GroupPlugin;
class GroupSession;
class UdpInPacket;


class IcqGroup : public Group {
public:
	IcqGroup(GroupPlugin *p, uint32 id);
	virtual ~IcqGroup();

	virtual OutPacket *createPacket(uint16 cmd);
	virtual void sendPacket(OutPacket *p, int dst, int src);
	virtual void restart();

	int addMember(UdpSession *s);
	void removeMember(UdpSession *s);
	void sendMessage(uint32 src, uint32 when, ICQ_STR &text);
	void start(uint32 uin);

	void onPacketReceived(UdpInPacket &in);

	ListHead hashItem;
	ListHead listItem;

	uint32 id;
	char name[MAX_GROUP_NAME_LEN + 1];
	char pass[MAX_GROUP_PASS_LEN + 1];
	int numMembers;
	UdpSession *members[MAX_GROUP_MEMBERS];
	uint32 startMask, groupMask;
	GroupPlugin *plugin;

private:
	int getPos(uint32 uin);

	void sendToAll(uint16 cmd, OutPacket *p, int src);
	void sendToMember(uint16 cmd, OutPacket *p, int dst);
	void onMemberEntered(int i);
	void onMemberExited(int i);

	GroupSession *session;

	DECLARE_SLAB(IcqGroup)
};


#endif
