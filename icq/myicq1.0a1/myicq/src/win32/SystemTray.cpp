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

// SystemTray.cpp: implementation of the SystemTray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "myicq.h"
#include "SystemTray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SystemTray::SystemTray()
{
	ZeroMemory(&tnid, sizeof(tnid));
	tnid.cbSize = sizeof(NOTIFYICONDATA);
}

SystemTray::~SystemTray()
{
	destroy();
}

void SystemTray::create(CWnd *wnd, UINT id, UINT msg)
{
	tnid.hWnd = *wnd;
	tnid.uID = id;
	tnid.uCallbackMessage = msg;

	tnid.uFlags = NIF_MESSAGE;
	Shell_NotifyIcon(NIM_ADD, &tnid);
}

void SystemTray::destroy()
{
	Shell_NotifyIcon(NIM_DELETE, &tnid);
}

void SystemTray::setIcon(HICON icon)
{
	tnid.uFlags = NIF_ICON;
	tnid.hIcon = icon;

	Shell_NotifyIcon(NIM_MODIFY, &tnid);
}

void SystemTray::setToolTip(LPCTSTR text)
{
	tnid.uFlags = NIF_TIP;
	lstrcpyn(tnid.szTip, text, sizeof(tnid.szTip));

	Shell_NotifyIcon(NIM_MODIFY, &tnid);
}