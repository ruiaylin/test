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

#ifndef _GROUP_H
#define _GROUP_H

#include "icqtypes.h"


class GroupMember {
public:
	uint32 uin;
	uint8 face;
	string nick;
};

class OutPacket;
class IcqLinkBase;

class Group {
public:
	virtual IcqLinkBase *getLink() = 0;
	virtual void destroy() = 0;
	virtual int getMyPos() = 0;
	virtual GroupMember *getMemberInfo(int i) = 0;
	virtual OutPacket *createPacket(uint16 cmd) = 0;
	virtual void sendPacket(OutPacket *out) = 0;
	virtual void start() = 0;
};


#endif
