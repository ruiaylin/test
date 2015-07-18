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

#ifndef _SESSION_H
#define _SESSION_H

#include "icqtypes.h"

class Session {
public:
	uint32 uin;
	uint32 status;
	uint32 ip;
	uint16 msgport;
	uint32 realip;
};

#endif
