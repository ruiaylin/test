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

#include "icqtypes.h"


char *QID::toString()
{
	static char buf[128];

	sprintf(buf, "%lu", uin);
	if (!domain.empty()) {
		strcat(buf, "@");
		strcat(buf, domain.c_str());
	}
	return buf;
}

bool QID::parse(const char *s)
{
	uin = 0;
	while (isdigit(*s)) {
		uin *= 10;
		uin += *s++ - '0';
	}

	if (!uin)
		return false;

	if (*s && *s++ != '@')
		return false;

	domain = s;	
	return true;
}
