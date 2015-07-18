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

// GroupSearchModePage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupSearchModePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchModePage property page

IMPLEMENT_DYNCREATE(CGroupSearchModePage, CPropertyPage)

CGroupSearchModePage::CGroupSearchModePage() : CPropertyPage(CGroupSearchModePage::IDD)
{
	//{{AFX_DATA_INIT(CGroupSearchModePage)
	m_mode = 0;
	m_groupID = 0;
	//}}AFX_DATA_INIT
}

CGroupSearchModePage::~CGroupSearchModePage()
{
}

void CGroupSearchModePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupSearchModePage)
	DDX_Radio(pDX, IDC_MODE_LIST, m_mode);
	DDX_Text(pDX, IDC_GROUP_ID, m_groupID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupSearchModePage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupSearchModePage)
	ON_BN_CLICKED(IDC_MODE_ID, OnModeID)
	ON_BN_CLICKED(IDC_MODE_LIST, OnModeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchModePage message handlers

BOOL CGroupSearchModePage::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}

LRESULT CGroupSearchModePage::OnWizardNext() 
{
	UpdateData();

	if (m_mode == 1)
		return IDD_GROUP_SEARCH_RESULT;

	return CPropertyPage::OnWizardNext();
}

void CGroupSearchModePage::OnModeID() 
{
	GetDlgItem(IDC_GROUP_ID)->EnableWindow(TRUE);
}

void CGroupSearchModePage::OnModeList() 
{
	GetDlgItem(IDC_GROUP_ID)->EnableWindow(FALSE);
}

void CGroupSearchModePage::OnCancel() 
{
	GetParent()->DestroyWindow();
}
