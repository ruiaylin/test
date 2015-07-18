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

#ifndef _ICQ_TYPES_H_
#define _ICQ_TYPES_H_

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

struct ICQ_STR {
	char *text;
	uint16 len;
};

struct QID {
	uint32 uin;
	ICQ_STR domain;
};

#ifdef _WIN32

#define ICQ_EXPORT	extern "C" __declspec(dllexport)

#else

#define ICQ_EXPORT	extern "C"

#endif

#endif
