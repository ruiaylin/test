/***************************************************************************
                          icqconfig.cpp  -  description
                             -------------------
    begin                : Tue Apr 9 2002
    copyright            : (C) 2002 by Zhang Yong
    email                : z-yong163@163.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include "icqconfig.h"

#define CONFIG_FILE		"myicq.cfg"

string IcqConfig::configDir;


void IcqConfig::getAllUsers(UinList &l)
{
	string pathName = configDir + CONFIG_FILE;
	FILE *file = fopen(pathName.c_str(), "r");
	char line[11];
	
	if (file) {
		while (fgets(line, sizeof(line), file))
			l.push_back(strtoul(line, NULL, 10));
		
		fclose(file);
	}
}

void IcqConfig::addUser(uint32 uin)
{
	UinList l;
	bool newUser = true;
	string pathName = configDir + CONFIG_FILE;
	FILE *file = fopen(pathName.c_str(), "r");
	
	if (file) {
		char line[11];	
		while (fgets(line, sizeof(line), file)) {
			uint32 d = strtoul(line, NULL, 10);
			if (d == uin) {
				newUser = false;
				l.push_front(d);
			} else
				l.push_back(d);
		}
		fclose(file);
	}
	if (newUser)
		l.push_front(uin);
		
	file = fopen(pathName.c_str(), "w");
	if (file) {
		UinList::iterator it;
		for (it = l.begin(); it != l.end(); ++it)
			fprintf(file, "%lu\n", *it);
		fclose(file);
	}
}
