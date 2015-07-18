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

#ifndef _ICQ_SESSION_H
#define _ICQ_SESSION_H

#include "socketlistener.h"
#include "icqtypes.h"

#define ICQ_SESSION_SERVER		"server"
#define ICQ_SESSION_MSG			"message"

class IcqLink;

class IcqSession : public SocketListener {
public:
	IcqSession(IcqLink *link, const char *name, uint32 uin);
	virtual ~IcqSession() {}

	string name;
	uint32 uin;

protected:
	IcqLink *icqLink;
};


#endif