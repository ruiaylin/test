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

// RegMiscDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RegMiscDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegMiscDlg property page

IMPLEMENT_DYNCREATE(CRegMiscDlg, CPropertyPage)

CRegMiscDlg::CRegMiscDlg() : CPropertyPage(CRegMiscDlg::IDD)
{
	//{{AFX_DATA_INIT(CRegMiscDlg)
	m_blood = 0;
	m_college = _T("");
	m_homepage = _T("http://");
	m_intro = _T("");
	m_profession = _T("");
	m_name = _T("");
	//}}AFX_DATA_INIT

	m_intro.LoadString(IDS_INTRO);
}

CRegMiscDlg::~CRegMiscDlg()
{
}

void CRegMiscDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegMiscDlg)
	DDX_Control(pDX, IDC_PROFESSION, m_profCombo);
	DDX_Control(pDX, IDC_BLOOD, m_bloodCombo);
	DDX_CBIndex(pDX, IDC_BLOOD, m_blood);
	DDX_Text(pDX, IDC_COLLEGE, m_college);
	DDX_Text(pDX, IDC_HOMEPAGE, m_homepage);
	DDX_Text(pDX, IDC_INTRO, m_intro);
	DDX_CBString(pDX, IDC_PROFESSION, m_profession);
	DDX_Text(pDX, IDC_NAME, m_name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegMiscDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CRegMiscDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegMiscDlg message handlers

BOOL CRegMiscDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	getApp()->loadText(m_bloodCombo, DATA_BLOOD);
	getApp()->loadText(m_profCombo, DATA_OCCUPATION);

	m_bloodCombo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
