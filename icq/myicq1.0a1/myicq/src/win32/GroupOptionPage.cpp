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

// GroupOptionPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupOptionPage.h"
#include "GroupInfoDlg.h"
#include "icqgroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupOptionPage property page

IMPLEMENT_DYNCREATE(CGroupOptionPage, CPropertyPage)

CGroupOptionPage::CGroupOptionPage() : CPropertyPage(CGroupOptionPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupOptionPage)
	m_msgOption = -1;
	//}}AFX_DATA_INIT
}

CGroupOptionPage::~CGroupOptionPage()
{
}

IcqGroup *CGroupOptionPage::getGroup()
{
	return ((CGroupInfoDlg *) GetParent())->group;
}

void CGroupOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupOptionPage)
	DDX_Radio(pDX, IDC_MSG_OPTION, m_msgOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupOptionPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupOptionPage)
	ON_BN_CLICKED(IDC_MODIFY, OnModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupOptionPage message handlers

void CGroupOptionPage::OnModify() 
{
	UpdateData();

	getGroup()->msgOption = m_msgOption;

	CString str;
	str.LoadString(IDS_SUCCESS_MODIFY);
	MessageBox(str);
}
