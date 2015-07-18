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

// GroupMembersPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupMembersPage.h"
#include "GroupInfoDlg.h"
#include "icqgroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupMembersPage property page

IMPLEMENT_DYNCREATE(CGroupMembersPage, CPropertyPage)

CGroupMembersPage::CGroupMembersPage() : CPropertyPage(CGroupMembersPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupMembersPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGroupMembersPage::~CGroupMembersPage()
{
}

void CGroupMembersPage::addMember(GroupMember *m)
{
	int n = m_memberList.GetItemCount();

	CString str;
	str.Format("%lu", m->uin);
	m_memberList.InsertItem(n, str, m->face);
	m_memberList.SetItemText(n, 1, m->nick.c_str());
}

void CGroupMembersPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupMembersPage)
	DDX_Control(pDX, IDC_MEMBER_LIST, m_memberList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupMembersPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupMembersPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupMembersPage message handlers

BOOL CGroupMembersPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_memberList.SetImageList(&getApp()->smallImageList, LVSIL_SMALL);

	CString str;
	str.LoadString(IDS_UIN);
	m_memberList.InsertColumn(0, str, LVCFMT_LEFT, 90);
	str.LoadString(IDS_NICK);
	m_memberList.InsertColumn(1, str, LVCFMT_CENTER, 120);

	IcqGroup *g = ((CGroupInfoDlg *) GetParent())->group;
	for (int i = 0; i < MAX_GROUP_MEMBERS; i++) {
		GroupMember *m = g->members[i];
		if (m)
			addMember(m);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
