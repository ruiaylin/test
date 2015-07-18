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

#include <time.h>
#include "msgsession.h"
#include "udppacket.h"
#include "icqlink.h"

uint32 MsgSession::msgID = 0xffffffff;


MsgSession::MsgSession(IcqLink *link, uint32 uin)
	: UdpSession(link, ICQ_SESSION_MSG, uin)
{
}

uint32 MsgSession::sendMessage(uint8 type, const char *text)
{
	time_t when = time(NULL);

	UdpOutPacket *out = createPacket(UDP_MSG_MESSAGE);
	*out << --msgID << type << (uint32) when << text;
	sendPacket(out);
	return msgID;
}

void MsgSession::onRecvMessage(UdpInPacket &in)
{
	uint32 id, when;
	uint8 type;
	const char *text;
	
	in >> id >> type >> when >> text;
	
	UdpOutPacket *out = createPacket(UDP_MSG_MESSAGE_ACK, in.getSeq());
	*out << id;
	sendDirect(out);
	delete out;
	
	icqLink->onRecvMessage(type, uin, when, text, false);
}

void MsgSession::onMessageAck(UdpInPacket &in)
{
	uint32 id;
	in >> id;
	icqLink->onAck(id);
}

bool MsgSession::onPacketReceived(UdpInPacket &in)
{
	if (!UdpSession::onPacketReceived(in))
		return false;
	
	uint16 cmd = in.getCmd();
	switch (cmd) {
	case UDP_MSG_MESSAGE:
		onRecvMessage(in);
		break;

	case UDP_MSG_MESSAGE_ACK:
		onMessageAck(in);
		break;

	default:
		return false;
	}
	return true;
}
