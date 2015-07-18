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

// GroupCreateWizard.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupCreateWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateWizard

IMPLEMENT_DYNAMIC(CGroupCreateWizard, CPropertySheet)

CGroupCreateWizard::CGroupCreateWizard()
: IcqWindow(WIN_GROUP_CREATE_WIZARD)
{
	SetWizardMode();
	m_psh.dwFlags |= PSH_USEHICON;
	m_psh.hIcon = getApp()->m_hIcon;

	AddPage(&typePage);
	AddPage(&basicPage);
	AddPage(&finishPage);

	Create();
}

CGroupCreateWizard::~CGroupCreateWizard()
{
}

BOOL CGroupCreateWizard::Create(CWnd *parent)
{
	DWORD dwStyle = WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE | WS_MINIMIZEBOX;
	DWORD dwExStyle = WS_EX_DLGMODALFRAME | WS_EX_APPWINDOW;
	return CPropertySheet::Create(parent, dwStyle, dwExStyle);
}

BEGIN_MESSAGE_MAP(CGroupCreateWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CGroupCreateWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateWizard message handlers

void CGroupCreateWizard::PostNcDestroy() 
{
	delete this;
}
