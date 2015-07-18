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

#include <string>
#include <list>
#include <vector>
#include <bitset>

using namespace std;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

#ifdef _WIN32
#define strncasecmp		_strnicmp
#define strcasecmp		_stricmp
#endif

#define ADMIN_UIN		10000


class QID {
public:
	QID() {
		uin = 0;
	}
	bool operator ==(const QID &qid) {
		return (uin == qid.uin && !strcasecmp(domain.c_str(), qid.domain.c_str()));
	}
	bool isAdmin() {
		return (uin == ADMIN_UIN && domain.empty());
	}
	char *toString();
	bool parse(const char *qid);

	uint32 uin;
	string domain;
};

typedef list<string> 	StrList;
typedef list<void *>	PtrList;
typedef list<uint32>	UinList;
typedef list<QID>		QIDList;
typedef vector<void *>	PtrArray;


#endif
