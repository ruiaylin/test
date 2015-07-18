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

// GroupBasicPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupBasicPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupBasicPage property page

IMPLEMENT_DYNCREATE(CGroupBasicPage, CPropertyPage)

CGroupBasicPage::CGroupBasicPage() : CPropertyPage(CGroupBasicPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupBasicPage)
	m_groupID = 0;
	m_groupName = _T("");
	//}}AFX_DATA_INIT
}

CGroupBasicPage::~CGroupBasicPage()
{
}

void CGroupBasicPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupBasicPage)
	DDX_Text(pDX, IDC_GROUP_ID, m_groupID);
	DDX_Text(pDX, IDC_GROUP_NAME, m_groupName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupBasicPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupBasicPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupBasicPage message handlers
