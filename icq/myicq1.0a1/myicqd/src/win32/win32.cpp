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

#include <windows.h>
#include "groupplugin.h"


GET_PLUGIN_INFO loadPlugin(const char *name)
{
	string s = name;
	s += ".dll";

	HMODULE hModule = LoadLibrary(s.c_str());
	if (!hModule)
		return false;

	return (GET_PLUGIN_INFO) GetProcAddress(hModule, "getPluginInfo");
}
