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

/*
 * By now, there are three types of plugins:
 *
 * 1) Net Plugin: sendfile, chat, ...
 * 2) Toy Plugin: alarm, desktop games, ...
 * 3) Exe Plugin: An existing third party program can be plugged into MyICQ
*/

#ifndef _PLUGIN_H
#define _PLUGIN_H

#include <string>

using namespace std;

// Plugin types
enum {
	ICQ_PLUGIN_NET,
	ICQ_PLUGIN_TOY,
	ICQ_PLUGIN_EXE
};

class PluginInfo {
public:
	PluginInfo() {
		type = -1;
		icon = 0;
	}

	int type;
	string name;	// Locale name
	void *icon;
};

#endif
