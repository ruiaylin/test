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

// RegCommDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RegCommDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegCommDlg property page

IMPLEMENT_DYNCREATE(CRegCommDlg, CPropertyPage)

CRegCommDlg::CRegCommDlg() : CPropertyPage(CRegCommDlg::IDD)
{
	//{{AFX_DATA_INIT(CRegCommDlg)
	m_address = _T("");
	m_email = _T("");
	m_tel = _T("");
	m_zipcode = _T("");
	//}}AFX_DATA_INIT
}

CRegCommDlg::~CRegCommDlg()
{
}

void CRegCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegCommDlg)
	DDX_Text(pDX, IDC_ADDRESS, m_address);
	DDX_Text(pDX, IDC_EMAIL, m_email);
	DDX_Text(pDX, IDC_TEL, m_tel);
	DDX_Text(pDX, IDC_ZIPCODE, m_zipcode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegCommDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CRegCommDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegCommDlg message handlers
