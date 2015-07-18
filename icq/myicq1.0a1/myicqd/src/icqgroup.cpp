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
#include "groupplugin.h"
#include "groupsession.h"
#include "udpsession.h"
#include "udppacket.h"
#include <memory.h>


IMPLEMENT_SLAB(IcqGroup, 32)


IcqGroup::IcqGroup(GroupPlugin *p, uint32 id)
{
	plugin = p;
	this->id = id;

	name[MAX_GROUP_NAME_LEN] = '\0';
	numMembers = 0;
	session = NULL;
	memset(members, 0, sizeof(members));

	startMask = 0;
	int n = p->info.maxMembers;
	groupMask = (n ? (1 << n) - 1 : 0);
}

IcqGroup::~IcqGroup()
{
	if (session)
		session->destroy();
}

int IcqGroup::getPos(uint32 uin)
{
	int n = plugin->getMaxMembers();
	for (int i = 0; i < n; i++) {
		if (members[i] && members[i]->uin == uin)
			return i;
	}
	return -1;
}

void IcqGroup::restart()
{
	startMask = 0;
}

void IcqGroup::start(uint32 uin)
{
	int i = getPos(uin);
	if (i < 0)
		return;

	uint32 m = (1 << i);
	if (startMask & m)
		return;

	startMask |= m;
	if ((startMask & groupMask) == groupMask) {
		sendToAll(UDP_GROUP_START, NULL, -1);
		session->onStart();
	}
}

void IcqGroup::onMemberEntered(int i)
{
	UdpSession *s = members[i];

	IcqOutPacket out;
	out << s->uin << s->face;
	out << s->nickname;
	sendToAll(UDP_SRV_ENTER_GROUP, &out, i);
}

void IcqGroup::onMemberExited(int i)
{
	IcqOutPacket out;
	out << members[i]->uin;
	sendToAll(UDP_SRV_EXIT_GROUP, &out, i);
}

int IcqGroup::addMember(UdpSession *s)
{
	int n = plugin->getMaxMembers();

	int i;
	for (i = 0; i < n; i++) {
		if (!members[i])
			break;
	}
	if (i >= n)
		return -1;

	members[i] = s;
	++numMembers;

	if (!session)
		session = plugin->createSession(this, numMembers);
	else
		session->onMemberEntered(i);

	onMemberEntered(i);

	return i;
}

void IcqGroup::removeMember(UdpSession *s)
{
	int n = plugin->getMaxMembers();
	int i;
	for (i = 0; i < n; i++) {
		if (members[i] == s)
			break;
	}
	if (i >= n)
		return;

	if (--numMembers <= 0) {
		plugin->destroyGroup(this);
		return;
	}

	if (session) {
		if (plugin->info.maxMembers) {
			session->destroy();
			session = NULL;
			restart();
		} else
			session->onMemberExited(i);
	}

	onMemberExited(i);

	members[i] = NULL;
}

void IcqGroup::sendMessage(uint32 src, uint32 when, ICQ_STR &text)
{
	int pos = getPos(src);
	if (pos < 0)
		return;
	
	IcqOutPacket out;
	out << src << when << text;

	sendToAll(UDP_GROUP_MESSAGE, &out, pos);
}

void IcqGroup::onPacketReceived(UdpInPacket &in)
{
	if (!session)
		return;
	
	uint32 uin = in.header.uin;

	int n = plugin->getMaxMembers();
	for (int i = 0; i < n; i++) {
		if (!members[i])
			continue;

		if (members[i]->uin == uin) {
			session->onPacketReceived(in, i);
			break;
		}
	}
}

void IcqGroup::sendToMember(uint16 cmd, OutPacket *p, int dst)
{
	UdpSession *s = members[dst];
	if (!s)
		return;

	UdpOutPacket *out = s->createPacket(cmd);
	*out << id;

	if (p) {
		IcqOutPacket *data = (IcqOutPacket *) p;
		out->writeData(data->getData(), data->getLength());
	}
	s->sendPacket(out);
}

void IcqGroup::sendToAll(uint16 cmd, OutPacket *p, int src)
{
	int n = plugin->getMaxMembers();

	for (int i = 0; i < n; i++) {
		if (i != src && members[i])
			sendToMember(cmd, p, i);
	}
}

OutPacket *IcqGroup::createPacket(uint16 cmd)
{
	static IcqOutPacket out;
	out.reset();
	out << cmd;
	return &out;
}

void IcqGroup::sendPacket(OutPacket *p, int dst, int src)
{
	if (dst < 0)
		sendToAll(UDP_GROUP_CMD, p, src);
	else if (dst < plugin->getMaxMembers())
		sendToMember(UDP_GROUP_CMD, p, dst);
}
