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

#include "tcpsessionbase.h"

#define TCP_SESSION_MSG		"message"

class IcqLink;

class MsgSession : public TcpSessionListener {
public:
	MsgSession(TcpSessionBase *tcp);

	uint32 sendMessage(uint8 type, const char *text);

private:
	virtual void onSend() {}
	virtual void onClose(bool prompt) {
		delete this;
	}
	virtual bool onPacketReceived(InPacket &in);

	void onRecvMessage(InPacket &in);
	void onMessageAck(InPacket &in);

	IcqLink *icqLink;
	TcpSessionBase *tcp;

	static uint32 msgID;
};

#endif
