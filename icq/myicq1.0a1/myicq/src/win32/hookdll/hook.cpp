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

#define DLL_EXPORT	extern "C" __declspec(dllexport)

static HHOOK hMouseHook;
static HHOOK hKeyboardHook;
static DWORD lastTickCount;


static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
		lastTickCount = GetTickCount();
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
		lastTickCount = GetTickCount();
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

static BOOL initHook(HINSTANCE hInst)
{
	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInst, 0);
	hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, hInst, 0);
	return (hKeyboardHook && hMouseHook);
}

static void destroyHook()
{
	if (hKeyboardHook)
		UnhookWindowsHookEx(hKeyboardHook);
	if (hMouseHook)
		UnhookWindowsHookEx(hMouseHook);
}


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
	lastTickCount = GetTickCount();

	if (fdwReason == DLL_PROCESS_ATTACH)
		return initHook(hInst);
	if (fdwReason == DLL_PROCESS_DETACH)
		destroyHook();

	return TRUE;
}

DLL_EXPORT DWORD getLastTickCount()
{
	return lastTickCount;
}
