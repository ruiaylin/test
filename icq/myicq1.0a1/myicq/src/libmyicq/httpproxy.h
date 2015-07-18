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

#ifndef _HTTP_PROXY_H
#define _HTTP_PROXY_H

#include "icqtypes.h"
#include "icqsocket.h"

class ProxyListener;
class ProxyInfo;

class HttpProxy : public SocketListener {
public:
	HttpProxy();
	virtual ~HttpProxy();

	void setListener(ProxyListener *l) {
		listener = l;
	}
	bool start(const char *destHost, ProxyInfo &proxy);

	// TCP Socket
	int sock;

private:
	virtual void onConnect(bool success);
	virtual bool onReceive();
	virtual void onClose();

	bool recvPacket();

	// The current status of this session
	int status;
	
	char buf[4096];
	int bufSize;

	// The proxy authentication stuff
	string uname;
	string passwd;

	// Destination host that we want to connect to
	string destHost;

	ProxyListener *listener;
};


#endif
