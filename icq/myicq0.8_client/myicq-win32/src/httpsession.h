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

#ifndef _HTTP_SESSION_H
#define _HTTP_SESSION_H

#include "socketlistener.h"
#include "icqtypes.h"

class SessionListener;
class ProxyInfo;

class HttpSession : public SocketListener {
public:
	HttpSession();
	~HttpSession();

	void setListener(SessionListener *l) {
		listener = l;
	}
	bool start(const char *destHost, ProxyInfo &proxy);

	int sock;
	char buf[4096];

private:
	virtual void onConnect(bool success);
	virtual bool onReceive();
	virtual void onClose();

	bool recvPacket();

	int status;
	int bufSize;
	string uname;
	string passwd;
	string destHost;
	SessionListener *listener;
};


#endif
