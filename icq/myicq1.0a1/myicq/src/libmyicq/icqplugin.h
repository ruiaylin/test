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

#ifndef _ICQ_PLUGIN_H
#define _ICQ_PLUGIN_H

#include "icqtypes.h"
#include "plugin.h"
#include <map>

using namespace std;

class IcqLinkBase;
class TcpSessionListener;
class TcpSessionBase;
class IcqContact;
class ProfileReader;
class ProfileWriter;

// The following functions should be supplied by the plugin module

// Get basic plugin information(type, name...)
typedef void (*GETPLUGININFO)(PluginInfo *info);
// Initialize a toy plugin
typedef bool (*INIT)(IcqLinkBase *link, ProfileReader *profile);
// Configure a toy plugin
typedef void (*CONFIGURE)(ProfileWriter *profile);
// About dialog about a plugin
typedef void (*ABOUT)();
// Create a tcp session listener(net plugin)
typedef TcpSessionListener *(*CREATESESSION)(TcpSessionBase *tcp);


class IcqPlugin {
public:
	PluginInfo info;
	string name;		// NOI18N
};

class NetPlugin : public IcqPlugin {
public:
	CREATESESSION createSession;
};

class ToyPlugin : public IcqPlugin {
public:
	INIT init;
	CONFIGURE configure;
	ABOUT about;
};

class ExePlugin : public IcqPlugin {
public:
	void execClient(IcqContact *c);
	void execServer(IcqContact *c);

	string clientCmd;
	string serverCmd;

private:
	void getCmd(string &cmd, const char *line, IcqContact *c);
	void execCmd(const char *cmd);
};


typedef map<string, IcqPlugin *> PLUGINMAP;

class PluginFactory {
public:
	static bool init();
	static void destroy();

	static bool registerPlugin(IcqPlugin *p);
	static IcqPlugin *getPlugin(const string &name) {
		return pluginMap[name];
	}
	static PtrList &getPluginList() {
		return pluginList;
	}

private:
	static PtrList pluginList;
	static PLUGINMAP pluginMap;
};

#endif
