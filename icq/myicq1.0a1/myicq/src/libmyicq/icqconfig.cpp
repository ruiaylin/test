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

#include <stdio.h>
#include "icqconfig.h"

#define CONFIG_FILE		"myicq.cfg"

string IcqConfig::configDir;


static const char *trim(char *s)
{
	while (isspace(*s))
		s++;

	int n = strlen(s);
	char *p = s + n;
	while (isspace(*--p))
		*p = '\0';

	return s;
}

void IcqConfig::getAllUsers(QIDList &l)
{
	string pathName = configDir + CONFIG_FILE;
	FILE *file = fopen(pathName.c_str(), "r");
	QID qid;
	
	if (file) {
		char line[128];
		while (fgets(line, sizeof(line), file)) {
			if (qid.parse(trim(line)))
				l.push_back(qid);
		}
		fclose(file);
	}
}

void IcqConfig::addUser(QID &newQID)
{
	StrList l;
	bool isNew = true;
	string pathName = configDir + CONFIG_FILE;
	FILE *file = fopen(pathName.c_str(), "r");

	if (file) {
		char line[128];
		QID qid;
		while (fgets(line, sizeof(line), file)) {
			qid.parse(trim(line));
			if (qid == newQID) {
				isNew = false;
				l.push_front(qid.toString());
			} else
				l.push_back(qid.toString());
		}
		fclose(file);
	}
	if (isNew)
		l.push_front(newQID.toString());

	file = fopen(pathName.c_str(), "w");
	if (file) {
		StrList::iterator it;
		for (it = l.begin(); it != l.end(); ++it)
			fprintf(file, "%s\n", (*it).c_str());
		fclose(file);
	}
}
