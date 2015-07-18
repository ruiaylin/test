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

#include "icqgroup.h"
#include "icqlink.h"
#include "udpsession.h"
#include "udppacket.h"
#include "groupplugin.h"


IcqGroup::IcqGroup(GroupPlugin *p, uint32 id)
{
	plugin = p;
	this->id = id;
	numMembers = 0;
	msgOption = GROUP_OPTION_MSG_NOTIFY;

	session = NULL;

	memset(members, 0, sizeof(members));
}

IcqGroup::~IcqGroup()
{
	for (int i = 0; i < MAX_GROUP_MEMBERS; i++) {
		if (members[i])
			delete members[i];
	}
}

void *IcqGroup::getIcon()
{
	if (!plugin)
		return NULL;

	return plugin->info.icon;
}

void IcqGroup::destroy()
{
	icqLink->exitGroup(this);
}

void IcqGroup::destroyAll()
{
	if (session) {
		session->destroy();
		session = NULL;
	}
	destroy();
}

IcqLinkBase *IcqGroup::getLink()
{
	return icqLink;
}

int IcqGroup::getPos(uint32 uin)
{
	int n = getMaxMembers();

	for (int i = 0; i < n; i++) {
		if (members[i]->uin == uin)
			return i;
	}
	return -1;
}

int IcqGroup::getMyPos()
{
	return getPos(icqLink->myInfo.qid.uin);
}

int IcqGroup::getMaxMembers()
{
	int n = plugin->info.numMembers;
	if (n <= 0 || n > MAX_GROUP_MEMBERS)
		return MAX_GROUP_MEMBERS;
	return n;
}

GroupMember *IcqGroup::getMemberInfo(int i)
{
	if (i < 0 || i >= MAX_GROUP_MEMBERS)
		return NULL;
	return members[i];
}

OutPacket *IcqGroup::createPacket(uint16 cmd)
{
	OutPacket *out = getUdpSession()->createGroupPacket(id);
	*out << cmd;
	return out;
}

void IcqGroup::sendPacket(OutPacket *out)
{
	getUdpSession()->sendPacket((UdpOutPacket *) out);
}

void IcqGroup::start()
{
	getUdpSession()->startGroup(id);
}

void IcqGroup::addMember(uint32 uin, uint8 face, const char *nick)
{
	int n = getMaxMembers();
	if (numMembers >= n)
		return;

	numMembers++;

	for (int i = 0; i < n; i++) {
		if (!members[i])
			break;
	}

	GroupMember *m = new GroupMember;
	m->uin = uin;
	m->face = face;
	m->nick = nick;
	members[i] = m;

	if (!session)
		session = plugin->createSession(this);
	if (session)
		session->onMemberEntered(i);
}

void IcqGroup::removeMember(uint32 uin)
{
	int n = getMaxMembers();
	for (int i = 0; i < n; i++) {
		if (members[i] && members[i]->uin == uin)
			break;
	}
	if (i >= n)
		return;

	numMembers--;

	if (session) {
		session->onMemberExited(i);

		if (plugin->info.numMembers) {
			session->destroy();
			session = NULL;
		}	
	}

	delete members[i];
	members[i] = NULL;
}

void IcqGroup::onGroupCmd(InPacket &in)
{
	if (session)
		session->onPacketReceived(in);
}

void IcqGroup::onStart()
{
	if (session)
		session->onStart();
}
