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

#ifndef _ICQ_PROFILE_H
#define _ICQ_PROFILE_H

#include "icqtypes.h"

class IcqProfile {
public:
	virtual void writeBool(const char *key, bool b) = 0;
	virtual void writeString(const char *key, const char *val) = 0;
	virtual void writeInt(const char *key, int val) = 0;
	virtual void writePassword(const char *key, const char *passwd) = 0;
	virtual bool readBool(const char *key) = 0;
	virtual void readString(const char *key, string &val) = 0;
	virtual int readInt(const char *key) = 0;
	virtual void readPassword(const char *key, string &passwd) = 0;
};

#endif
