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

// SearchWizard.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SearchWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchWizard

IMPLEMENT_DYNAMIC(CSearchWizard, CPropertySheet)

CSearchWizard::CSearchWizard() : IcqWindow(WIN_SEARCH_WIZARD)
{
	SetWizardMode();
	m_psh.dwFlags |= PSH_USEHICON;
	m_psh.hIcon = getApp()->m_hIcon;

	AddPage(&modeDlg);
	AddPage(&customDlg);
	AddPage(&resultDlg);
	AddPage(&addDlg);
}

CSearchWizard::~CSearchWizard()
{
}


BOOL CSearchWizard::Create(CWnd *pParentWnd)
{
	DWORD dwStyle = WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE | WS_MINIMIZEBOX;
	DWORD dwExStyle = WS_EX_DLGMODALFRAME | WS_EX_APPWINDOW;
	return CPropertySheet::Create(pParentWnd, dwStyle, dwExStyle);
}

BEGIN_MESSAGE_MAP(CSearchWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CSearchWizard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchWizard message handlers

void CSearchWizard::PostNcDestroy() 
{
	delete this;
}
