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

// alarm.h : main header file for the ALARM DLL
//

#if !defined(AFX_ALARM_H__39277851_D33F_4C27_B316_87D8D06649D5__INCLUDED_)
#define AFX_ALARM_H__39277851_D33F_4C27_B316_87D8D06649D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAlarmApp
// See alarm.cpp for the implementation of this class
//

class CAlarmApp : public CWinApp
{
public:
	CAlarmApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAlarmApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARM_H__39277851_D33F_4C27_B316_87D8D06649D5__INCLUDED_)
