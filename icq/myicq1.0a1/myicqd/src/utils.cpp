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

#include "utils.h"
#include "icqsocket.h"


const char *strLowerCase(char *str)
{
	for (char *p = str; *p; p++) {
		if (*p >= 'A' && *p <= 'Z')
			*p += 'a' - 'A';
	}

	return str;
}

const char *myicq_inet_ntoa(uint32 ip)
{
	in_addr addr;
	addr.s_addr = ip;
	return inet_ntoa(addr);
}
