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

// SearchCustomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SearchCustomDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchCustomDlg property page

IMPLEMENT_DYNCREATE(CSearchCustomDlg, CPropertyPage)

CSearchCustomDlg::CSearchCustomDlg() : CPropertyPage(CSearchCustomDlg::IDD)
{
	//{{AFX_DATA_INIT(CSearchCustomDlg)
	m_email = _T("");
	m_nick = _T("");
	m_uin = 0;
	//}}AFX_DATA_INIT
}

CSearchCustomDlg::~CSearchCustomDlg()
{
}

void CSearchCustomDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchCustomDlg)
	DDX_Text(pDX, IDC_EMAIL, m_email);
	DDX_Text(pDX, IDC_NICK, m_nick);
	DDX_Text(pDX, IDC_UIN, m_uin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchCustomDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSearchCustomDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchCustomDlg message handlers

BOOL CSearchCustomDlg::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}
