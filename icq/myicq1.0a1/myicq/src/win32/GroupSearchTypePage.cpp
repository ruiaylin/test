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

// GroupSearchTypePage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupSearchTypePage.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchTypePage property page

IMPLEMENT_DYNCREATE(CGroupSearchTypePage, CPropertyPage)

CGroupSearchTypePage::CGroupSearchTypePage() : CPropertyPage(CGroupSearchTypePage::IDD)
{
	//{{AFX_DATA_INIT(CGroupSearchTypePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGroupSearchTypePage::~CGroupSearchTypePage()
{
}

int CGroupSearchTypePage::getGroupType()
{
	int type = -1;
	POSITION pos = m_typeList.GetFirstSelectedItemPosition();
	if (pos)
		type = m_typeList.GetNextSelectedItem(pos);
	return type;
}

void CGroupSearchTypePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupSearchTypePage)
	DDX_Control(pDX, IDC_TYPE_LIST, m_typeList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupSearchTypePage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupSearchTypePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchTypePage message handlers

BOOL CGroupSearchTypePage::OnInitDialog() 
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

BOOL CGroupSearchTypePage::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}

LRESULT CGroupSearchTypePage::OnWizardNext() 
{
	int i = getGroupType();
	if (i < 0) {
		myMessageBox(IDS_SELECT_RECORD, IDS_SORRY, this);
		return -1;
	}

	return CPropertyPage::OnWizardNext();
}
