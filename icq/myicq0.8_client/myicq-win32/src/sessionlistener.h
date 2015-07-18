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

#ifndef _SESSION_LISTENER_H
#define _SESSION_LISTENER_H

class UdpInPacket;

class SessionListener {
public:
	virtual void sessionFinished(bool success) = 0;
	virtual bool onPacketReceived(UdpInPacket &in) { return true; }
};


#endif
