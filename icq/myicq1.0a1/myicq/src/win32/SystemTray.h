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

// SystemTray.h: interface for the SystemTray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMTRAY_H__A0CF22BB_D876_40F2_AE33_008BB49B8B24__INCLUDED_)
#define AFX_SYSTEMTRAY_H__A0CF22BB_D876_40F2_AE33_008BB49B8B24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SystemTray
{
public:
	SystemTray();
	~SystemTray();

	void create(CWnd *wnd, UINT id, UINT msg);
	void destroy();

	void setIcon(HICON icon);
	void setToolTip(LPCTSTR text);

private:
	NOTIFYICONDATA tnid;
};

#endif // !defined(AFX_SYSTEMTRAY_H__A0CF22BB_D876_40F2_AE33_008BB49B8B24__INCLUDED_)
