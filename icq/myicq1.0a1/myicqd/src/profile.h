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

#ifndef _PROFILE_H
#define _PROFILE_H

#include <string>
#include <map>

using namespace std;

typedef map<string, string> StringMap;

class Section;

typedef bool (*ON_SECTION_PARSED)(Section &sec);


class Section {
public:
	const char *getName() {
		return name.c_str();
	}

	const char *getString(const char *name, const char *def = "");
	int getInteger(const char *name, int def);

	friend bool parseProfile(const char *name, ON_SECTION_PARSED onSectionParsed);

private:
	string name;
	StringMap pairs;
};


#endif
