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

#ifndef _SOCKS_PROXY_H
#define _SOCKS_PROXY_H

#include "icqtypes.h"
#include "icqsocket.h"

class ProxyListener;
class ProxyInfo;

class SocksProxy : public SocketListener {
public:
	SocksProxy();
	virtual ~SocksProxy();

	void setListener(ProxyListener *l) {
		listener = l;
	}
	bool start(ProxyInfo &info, int sock = -1);

	int tcpSock;

private:
	virtual void onConnect(bool success);
	virtual bool onReceive();
	virtual void onClose();

	int status;
	string uname;
	string passwd;
	int udpSock;
	ProxyListener *listener;
};


#endif
