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

#ifndef _SOCKET_LISTENER_H
#define _SOCKET_LISTENER_H

class SocketListener {
public:
	virtual bool onReceive() = 0;
	virtual void onConnect(bool success) {}
	virtual void onSend() {}
	virtual void onClose() {}
};

#endif