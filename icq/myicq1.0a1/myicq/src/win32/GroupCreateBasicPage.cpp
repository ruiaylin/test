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

// GroupCreateBasicPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupCreateBasicPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateBasicPage property page

IMPLEMENT_DYNCREATE(CGroupCreateBasicPage, CPropertyPage)

CGroupCreateBasicPage::CGroupCreateBasicPage() : CPropertyPage(CGroupCreateBasicPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupCreateBasicPage)
	m_groupName = _T("");
	m_passwd = _T("");
	//}}AFX_DATA_INIT
}

CGroupCreateBasicPage::~CGroupCreateBasicPage()
{
}

void CGroupCreateBasicPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupCreateBasicPage)
	DDX_Text(pDX, IDC_GROUP_NAME, m_groupName);
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupCreateBasicPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupCreateBasicPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateBasicPage message handlers

BOOL CGroupCreateBasicPage::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}
