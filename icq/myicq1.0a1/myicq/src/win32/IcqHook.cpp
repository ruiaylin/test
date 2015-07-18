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
#include "icqhook.h"

typedef DWORD (*GETLASTTICK)();

static HMODULE hookModule;
static GETLASTTICK getLastTickCount;


bool IcqHook::install()
{
	IcqHook::uninstall();

	hookModule = LoadLibrary("myicqhook.dll");
	if (!hookModule)
		return false;

	getLastTickCount = (GETLASTTICK) GetProcAddress(hookModule, "getLastTickCount");
	return (getLastTickCount != NULL);
}

void IcqHook::uninstall()
{
	if (hookModule) {
		FreeLibrary(hookModule);
		hookModule = NULL;
	}
}

DWORD IcqHook::getLastTick()
{
	return (getLastTickCount ? getLastTickCount() : 0);
}
