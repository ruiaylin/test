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

// GroupInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupInfoDlg.h"
#include "icqgroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoDlg

IMPLEMENT_DYNAMIC(CGroupInfoDlg, CPropertySheet)

CGroupInfoDlg::CGroupInfoDlg(IcqGroup *g)
	:CPropertySheet(IDS_GROUP_INFO)
{
	group = g;

	basicPage.m_groupID = g->id;
	basicPage.m_groupName = g->name.c_str();
	optionPage.m_msgOption = g->msgOption;
	
	AddPage(&basicPage);
	AddPage(&membersPage);
	AddPage(&optionPage);

	DWORD dwStyle = WS_VISIBLE | WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_MINIMIZEBOX;
	Create(NULL, dwStyle);
}

CGroupInfoDlg::~CGroupInfoDlg()
{
}


BEGIN_MESSAGE_MAP(CGroupInfoDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CGroupInfoDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoDlg message handlers

void CGroupInfoDlg::PostNcDestroy() 
{
	delete this;	
}
