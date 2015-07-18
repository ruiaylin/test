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

#ifndef _PROXY_LISTENER_H
#define _PROXY_LISTENER_H

#include "icqtypes.h"

class UdpInPacket;
class DBInStream;
class DBOutStream;

class ProxyListener {
public:
	virtual void onEstablished(bool success) = 0;
	virtual bool onPacketReceived(UdpInPacket &in) { return true; }
};

class ProxyInfo {
public:
	ProxyInfo();

	void load(DBInStream &in);
	void save(DBOutStream &out);

	string host;
	uint16 port;
	string username;
	string passwd;
	uint8 resolve;
};

#endif
