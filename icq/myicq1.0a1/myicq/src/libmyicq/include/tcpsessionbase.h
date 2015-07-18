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
	// Called when data can be sent without blocking
	virtual void onSend() = 0;

	// If prompt is true, the remote has closed this session, otherwise,
	// the main module asks us to shutdown explicitly
	virtual void onClose(bool prompt) = 0;

	// Called when a new packet arrives.
	virtual bool onPacketReceived(InPacket &in) = 0;
};

class TcpSessionBase {
public:
	// Return the IcqLinkBase object
	virtual IcqLinkBase *getLink() = 0;

	// Return the socket descriptor in this session.
	virtual int getSocket() = 0;

	// Return the remote UIN of this session.
	virtual void getRemoteQID(QID &qid) = 0;

	// Destroy this session
	virtual void destroy() = 0;

	// Shutdown this session, but do not destroy it
	virtual void shutdown() = 0;

	// Determine if this session a send session. A send session is the one
	// who makes the active call.
	virtual bool isSendSession() = 0;

	// Enable write events to be notified when you can send more data.
	virtual void enableWrite() = 0;

	// Create a packet using cmd
	virtual OutPacket *createPacket(uint16 cmd) = 0;

	// Send the packet to the remote
	virtual bool sendPacket(OutPacket *out) = 0;
};

#endif
