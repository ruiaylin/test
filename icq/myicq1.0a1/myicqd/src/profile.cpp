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

#include "profile.h"
#include <stdio.h>
#include <ctype.h>


const char *Section::getString(const char *name, const char *def)
{
	StringMap::iterator it = pairs.find(name);
	if (it == pairs.end())
		return def;
	return (*it).second.c_str();
}

int Section::getInteger(const char *name, int def)
{
	const char *val = getString(name, NULL);
	if (!val)
		return def;
	return atoi(val);
}


inline void skipWhiteSpace(const char *&p)
{
	while (*p && (*p == ' ' || *p == '\t'))
		p++;
}

static bool parseSection(string &name, const char *line)
{
	char buf[256];
	char *p = buf;
	
	line++;
	while (*line && *line != ']')
		*p++ = *line++;

	*p = '\0';
	if (!*line)
		return false;

	name = buf;
	return true;
}

static bool parsePair(StringMap &pairs, const char *line)
{
	char name[256];
	char val[256];
	char *p = name;

	while (*line && *line != '=' && !isspace(*line))
		*p++ = *line++;
	*p = '\0';
	if (!*name)
		return false;
	
	skipWhiteSpace(line);
	if (*line != '=')
		return false;

	skipWhiteSpace(++line);

	p = val;
	while (*line && *line != '\r' && *line != '\n')
		*p++ = *line++;
	*p = '\0';

	if (*val)
		pairs[name] = val;
		
	return true;
}


bool parseProfile(const char *name, ON_SECTION_PARSED onSectionParsed)
{
	FILE *file = fopen(name, "r");
	if (!file)
		return false;

	Section sec;
	char line[256];

	while (fgets(line, sizeof(line), file)) {
		if (*line == '#')
			continue;

		if (*line == '[') {
			if (!sec.name.empty()) {
				onSectionParsed(sec);
				sec.name = "";
				sec.pairs.clear();
			}
			parseSection(sec.name, line);
		} else
			parsePair(sec.pairs, line);
	}

	if (!sec.name.empty())
		onSectionParsed(sec);

	fclose(file);
	return true;
}
