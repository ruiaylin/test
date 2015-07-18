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

#include "profile.h"
#include <string>
#include <map>

class IcqProfile : public ProfileReader, public ProfileWriter {
public:
	virtual ~IcqProfile() {}

	void setFileName(const char *name) { fileName = name; }
	void setSectionName(const char *name);
	void flush();

	virtual const char *readString(const char *key, const char *def);
	virtual int readInt(const char *key, int def);
	virtual void readPassword(const char *key, string &passwd);

	virtual void writeString(const char *key, const char *val) { pairs[key] = val; }
	virtual void writeInt(const char *key, int val);
	virtual void writePassword(const char *key, const char *passwd);

private:
	string fileName;
	string sectionName;
	map<string, string> pairs;
};

#endif
