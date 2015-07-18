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

#ifndef _TCP_SESSION_BASE
#define _TCP_SESSION_BASE

#include "icqtypes.h"

class OutPacket;
class InPacket;
class IcqLinkBase;

class TcpSessionListener {
public:
	virtual bool onTcpEstablished() { return false; }
	virtual bool onReceive() { return false; }
	virtual void onSend() = 0;
	virtual void onClose() = 0;
	virtual bool onPacketReceived(InPacket &in) = 0;
};

class TcpSessionBase {
public:
	virtual IcqLinkBase *getLink() = 0;
	virtual int getSocket() = 0;
	virtual uint32 getRemoteUIN() = 0;
	virtual void destroy() = 0;
	virtual void shutdown() = 0;
	virtual bool isSendSession() = 0;
	virtual void enableWrite() = 0;
	virtual OutPacket *createPacket(uint16 cmd) = 0;
	virtual bool sendPacket(OutPacket *out) = 0;
};

#endif