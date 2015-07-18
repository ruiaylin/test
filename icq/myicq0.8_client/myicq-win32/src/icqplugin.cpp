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

#include "icqplugin.h"
#include "icqsocket.h"
#include "icqclient.h"

PtrList PluginFactory::pluginList;
PLUGINMAP PluginFactory::pluginMap;


void ExePlugin::execClient(IcqContact *c)
{
	string cmd;
	getCmd(cmd, clientCmd.c_str(), c);
	execCmd(cmd.c_str());
}

void ExePlugin::execServer(IcqContact *c)
{
	string cmd;
	getCmd(cmd, clientCmd.c_str(), c);
	execCmd(cmd.c_str());
}

void ExePlugin::getCmd(string &cmd, const char *line, IcqContact *c)
{
	char buf[1024];
	char *p = buf;

	while (*line && !isspace(*line))
		*p++ = *line++;

	while (*line) {
		if (*line == '%') {
			++line;
			if (*line != '%') {
				char word[256];
				char *tmp = word;
				while (*line && !isspace(*line))
					*tmp++ = *line++;
				*tmp = '\0';

				char s[128];
				if (stricmp(word, "ip") == 0) {
					in_addr addr;
					addr.s_addr = htonl(c->ip);
					strcpy(s, inet_ntoa(addr));
				} else
					strcpy(s, word);

				int n = strlen(s);
				memcpy(p, s, n);
				p += n;
			}
		}

		if (*line)
			*p++ = *line++;
	}
	*p = '\0';

	cmd = buf;
}

bool PluginFactory::init()
{
	return true;
}

void PluginFactory::destroy()
{
	PtrList::iterator it;
	for (it = pluginList.begin(); it != pluginList.end(); ++it)
		delete (IcqPlugin *) *it;
	pluginList.clear();
}

bool PluginFactory::registerPlugin(IcqPlugin *p)
{
	if (!pluginMap.insert(PLUGINMAP::value_type(p->name, p)).second)
		return false;

	pluginList.push_back(p);
	return true;
}
