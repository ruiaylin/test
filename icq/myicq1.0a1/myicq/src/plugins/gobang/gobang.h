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

// gobang.h : main header file for the GOBANG DLL
//

#if !defined(AFX_GOBANG_H__2E2FA16B_1D70_4ED1_BB23_B130E0E8B0E7__INCLUDED_)
#define AFX_GOBANG_H__2E2FA16B_1D70_4ED1_BB23_B130E0E8B0E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGobangApp
// See gobang.cpp for the implementation of this class
//

class CGobangApp : public CWinApp
{
public:
	CGobangApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGobangApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGobangApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOBANG_H__2E2FA16B_1D70_4ED1_BB23_B130E0E8B0E7__INCLUDED_)
