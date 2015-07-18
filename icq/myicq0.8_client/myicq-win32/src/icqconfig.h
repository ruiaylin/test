/***************************************************************************
                          icqconfig.h  -  description
                             -------------------
    begin                : Tue Apr 9 2002
    copyright            : (C) 2002 by Zhang Yong
    email                : z-yong163@163.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ICQ_CONFIG_H
#define _ICQ_CONFIG_H

#include "icqtypes.h"

class IcqConfig {
public:
	static void setDir(const char *dir) {
		configDir = dir;
	}
	static void addUser(uint32 uin);
	static void getAllUsers(UinList &l);
	
private:
	static string configDir;
};

#endif
