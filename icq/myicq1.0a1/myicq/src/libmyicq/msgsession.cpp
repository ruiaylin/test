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
#include "tcpsessionbase.h"
#include "msgsession.h"
#include "packet.h"
#include "icqlink.h"

enum {
	TCP_MSG_MESSAGE = 0x1000,
	TCP_MSG_MESSAGE_ACK,
};

uint32 MsgSession::msgID = 0xFFffFFff;


MsgSession::MsgSession(TcpSessionBase *tcp)
{
	this->tcp = tcp;
	icqLink = (IcqLink *) tcp->getLink();
}

uint32 MsgSession::sendMessage(uint8 type, const char *text)
{
	time_t when = time(NULL);

	OutPacket *out = tcp->createPacket(TCP_MSG_MESSAGE);
	*out << --msgID << type << (uint32) when << text;
	tcp->sendPacket(out);
	return msgID;
}

void MsgSession::onRecvMessage(InPacket &in)
{
	uint32 id, when;
	uint8 type;
	const char *text;

	in >> id >> type >> when >> text;

	OutPacket *out = tcp->createPacket(TCP_MSG_MESSAGE_ACK);
	*out << id;
	tcp->sendPacket(out);

	QID qid;
	tcp->getRemoteQID(qid);
	icqLink->onRecvMessage(type, qid, when, text, false);
}

void MsgSession::onMessageAck(InPacket &in)
{
	uint32 id;
	in >> id;
	icqLink->onAck(id);
}

bool MsgSession::onPacketReceived(InPacket &in)
{
	uint16 cmd = in.getCmd();
	switch (cmd) {
	case TCP_MSG_MESSAGE:
		onRecvMessage(in);
		break;

	case TCP_MSG_MESSAGE_ACK:
		onMessageAck(in);
		break;

	default:
		return false;
	}
	return true;
}
