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

#ifndef _SESSION_HASH_H
#define _SESSION_HASH_H

#include "icqtypes.h"

class UdpSession;
class IcqOutPacket;

class SessionHash {
public:
	static UdpSession *get(uint32 uin);
	static UdpSession *get(uint32 ip, uint16 port);
	static void put(UdpSession *p, uint32 ip, uint16 port);
	static void put(UdpSession *p, uint32 uin);
	static void random(IcqOutPacket &out, int n);
};

#endif
