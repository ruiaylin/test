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

#ifndef _MSG_SESSION_H
#define _MSG_SESSION_H

#include "udpsession.h"


class MsgSession : public UdpSession {
public:
	MsgSession(IcqLink *link, uint32 uin);

	uint32 sendMessage(uint8 type, const char *text);

private:
	bool onPacketReceived(UdpInPacket &);
	void onRecvMessage(UdpInPacket &);
	void onMessageAck(UdpInPacket &);

	static uint32 msgID;
};

#endif
