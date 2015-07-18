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

// gobang.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "gobang.h"
#include "groupsession.h"
#include "GobangDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CGobangApp

BEGIN_MESSAGE_MAP(CGobangApp, CWinApp)
	//{{AFX_MSG_MAP(CGobangApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGobangApp construction

CGobangApp::CGobangApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGobangApp object

CGobangApp theApp;


static GroupSession *createGroup(Group *g)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return new CGobangDlg(g);
}


#define ICQ_EXPORT	extern "C" __declspec(dllexport)


ICQ_EXPORT getPluginInfo(GroupPluginInfo *info)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	info->createGroup = createGroup;
	info->icon = AfxGetApp()->LoadIcon(IDI_GOBANG);
	info->numMembers = 2;
}
