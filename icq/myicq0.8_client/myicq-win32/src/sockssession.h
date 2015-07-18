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

#ifndef _SOCKS_SESSION_H
#define _SOCKS_SESSION_H

#include "socketlistener.h"
#include "icqtypes.h"

class SessionListener;
class ProxyInfo;

class SocksSession : public SocketListener {
public:
	SocksSession();
	~SocksSession();

	void setListener(SessionListener *l) {
		listener = l;
	}
	bool start(ProxyInfo &socks, uint16 port = 0);

	int sock;
	uint32 socksIP;		// network order
	uint16 socksPort;

private:
	virtual void onConnect(bool success);
	virtual bool onReceive();
	virtual void onClose();

	uint16 localPort;	// network order

	int status;
	string uname;
	string passwd;
	int udpSock;
	SessionListener *listener;
};


#endif
