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

// GroupCreateTypePage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupCreateTypePage.h"
#include "GroupCreateWizard.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateTypePage property page

IMPLEMENT_DYNCREATE(CGroupCreateTypePage, CPropertyPage)

CGroupCreateTypePage::CGroupCreateTypePage() : CPropertyPage(CGroupCreateTypePage::IDD)
{
	//{{AFX_DATA_INIT(CGroupCreateTypePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGroupCreateTypePage::~CGroupCreateTypePage()
{
}

void CGroupCreateTypePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupCreateTypePage)
	DDX_Control(pDX, IDC_TYPE_LIST, m_typeList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupCreateTypePage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupCreateTypePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateTypePage message handlers

BOOL CGroupCreateTypePage::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}

BOOL CGroupCreateTypePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_typeList.SetExtendedStyle(m_typeList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	CString str;
	str.LoadString(IDS_GROUP_TYPE);
	m_typeList.InsertColumn(0, str, LVCFMT_LEFT, 120);
	str.LoadString(IDS_PLUGIN);
	m_typeList.InsertColumn(1, str, LVCFMT_CENTER, 120);

	PtrArray &a = icqLink->groupTypes;
	int n = a.size();
	for (int i = 0; i < n; i++) {
		GROUP_TYPE_INFO *p = (GROUP_TYPE_INFO *) a[i];

		m_typeList.InsertItem(i, p->displayName.c_str());
		m_typeList.SetItemText(i, 1, p->name.c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGroupCreateTypePage::OnCancel() 
{
	GetParent()->DestroyWindow();
}

LRESULT CGroupCreateTypePage::OnWizardNext() 
{
	CGroupCreateWizard *wiz = (CGroupCreateWizard *) GetParent();

	POSITION pos = m_typeList.GetFirstSelectedItemPosition();
	if (!pos) {
		myMessageBox(IDS_SELECT_RECORD, IDS_SORRY, this);
		return -1;
	}

	int i = m_typeList.GetNextSelectedItem(pos);
	wiz->groupType = i;
	wiz->groupPlugin = m_typeList.GetItemText(i, 1);

	return CPropertyPage::OnWizardNext();
}
