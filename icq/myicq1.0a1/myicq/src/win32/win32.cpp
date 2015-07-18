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

#include "stdafx.h"
#include "myicq.h"
#include "icqplugin.h"
#include "icqclient.h"
#include "groupplugin.h"
#include <mmsystem.h>


void ExePlugin::execCmd(const char *cmd)
{
	WinExec(cmd, SW_SHOW);
}

bool IcqOption::playSound(const char *file)
{
	return ::PlaySound(file, NULL, SND_FILENAME | SND_ASYNC);
}

GroupPlugin *GroupPlugin::load(const char *name)
{
	CString path = getApp()->rootDir + "groups\\" + name + ".dll";
	HMODULE hModule = LoadLibrary(path);
	if (!hModule)
		return NULL;

	GET_PLUGIN_INFO getPluginInfo =
		(GET_PLUGIN_INFO) GetProcAddress(hModule, "getPluginInfo");
	if (!getPluginInfo) {
		FreeLibrary(hModule);
		return NULL;
	}

	GroupPlugin *p = new GroupPlugin;
	getPluginInfo(&p->info);

	return p;
}
