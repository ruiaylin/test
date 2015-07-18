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

#include "icqprofile.h"

class WinProfile : public IcqProfile {
public:
	virtual void writeBool(const char *key, bool b);
	virtual void writeString(const char *key, const char *val);
	virtual void writeInt(const char *key, int val);
	virtual void writePassword(const char *key, const char *passwd);
	virtual bool readBool(const char *key);
	virtual void readString(const char *key, string &val);
	virtual int readInt(const char *key);
	virtual void readPassword(const char *key, string &passwd);

	CString fileName;
	CString sectionName;
};
