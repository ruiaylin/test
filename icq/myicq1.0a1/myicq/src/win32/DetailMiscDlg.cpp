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

// DetailMiscDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "DetailMiscDlg.h"
#include "icqclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDetailMiscDlg property page

IMPLEMENT_DYNCREATE(CDetailMiscDlg, CPropertyPage)

CDetailMiscDlg::CDetailMiscDlg() : CPropertyPage(CDetailMiscDlg::IDD)
{
	//{{AFX_DATA_INIT(CDetailMiscDlg)
	m_blood = 0;
	m_college = _T("");
	m_homepage = _T("");
	m_intro = _T("");
	m_profession = _T("");
	m_name = _T("");
	//}}AFX_DATA_INIT
}

CDetailMiscDlg::~CDetailMiscDlg()
{
}

void CDetailMiscDlg::enableAll(BOOL enable)
{
	((CEdit *) GetDlgItem(IDC_NAME))->SetReadOnly(!enable);
	((CEdit *) GetDlgItem(IDC_COLLEGE))->SetReadOnly(!enable);
	((CEdit *) GetDlgItem(IDC_HOMEPAGE))->SetReadOnly(!enable);
	((CEdit *) GetDlgItem(IDC_INTRO))->SetReadOnly(!enable);
	GetDlgItem(IDC_BLOOD)->EnableWindow(enable);
	GetDlgItem(IDC_PROFESSION)->EnableWindow(enable);
}

void CDetailMiscDlg::setData(IcqInfo *info)
{
	m_name = info->name.c_str();
	m_blood = info->blood;
	m_college = info->college.c_str();
	m_profession = info->profession.c_str();
	m_homepage = info->homepage.c_str();
	m_intro = info->intro.c_str();
	UpdateData(FALSE);
}

void CDetailMiscDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDetailMiscDlg)
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


BEGIN_MESSAGE_MAP(CDetailMiscDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CDetailMiscDlg)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetailMiscDlg message handlers

void CDetailMiscDlg::OnUpdate() 
{
	GetParent()->SendMessage(WM_COMMAND, IDC_UPDATE);
}

void CDetailMiscDlg::OnCancel() 
{
	GetParent()->DestroyWindow();
}

BOOL CDetailMiscDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	getApp()->loadText(m_bloodCombo, DATA_BLOOD);
	getApp()->loadText(m_profCombo, DATA_OCCUPATION);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
