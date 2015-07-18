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

// RegBasicDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RegBasicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegBasicDlg property page

IMPLEMENT_DYNCREATE(CRegBasicDlg, CPropertyPage)

CRegBasicDlg::CRegBasicDlg() : CPropertyPage(CRegBasicDlg::IDD)
{
	//{{AFX_DATA_INIT(CRegBasicDlg)
	m_province = _T("");
	m_city = _T("");
	m_country = _T("");
	m_gender = 0;
	m_passwd = _T("");
	m_passwdAgain = _T("");
	m_age = 0;
	m_nick = _T("");
	m_pic = -1;
	//}}AFX_DATA_INIT
}

CRegBasicDlg::~CRegBasicDlg()
{
}

void CRegBasicDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegBasicDlg)
	DDX_Control(pDX, IDC_PROVINCE, m_provCombo);
	DDX_Control(pDX, IDC_COUNTRY, m_countryCombo);
	DDX_Control(pDX, IDC_GENDER, m_genderCombo);
	DDX_Control(pDX, IDC_FACE, m_faceCombo);
	DDX_CBString(pDX, IDC_PROVINCE, m_province);
	DDX_Text(pDX, IDC_CITY, m_city);
	DDX_CBString(pDX, IDC_COUNTRY, m_country);
	DDX_CBIndex(pDX, IDC_GENDER, m_gender);
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
	DDX_Text(pDX, IDC_PASSWD_AGAIN, m_passwdAgain);
	DDX_Text(pDX, IDC_AGE, m_age);
	DDX_Text(pDX, IDC_NICK, m_nick);
	DDX_CBIndex(pDX, IDC_FACE, m_pic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegBasicDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CRegBasicDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegBasicDlg message handlers

BOOL CRegBasicDlg::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}

BOOL CRegBasicDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_faceCombo.SetImageList(&getApp()->largeImageList);

	COMBOBOXEXITEM cbi;
	ZeroMemory(&cbi, sizeof(cbi));
	for (int i = 0; i < NR_FACES; i++) {
		cbi.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		cbi.iItem = i;
		cbi.iImage =
		cbi.iSelectedImage = i;
	
		m_faceCombo.InsertItem(&cbi);
	}
	m_faceCombo.SetCurSel(0);

	getApp()->loadText(m_genderCombo, DATA_GENDER);
	getApp()->loadText(m_countryCombo, DATA_COUNTRY);
	getApp()->loadText(m_provCombo, DATA_PROVINCE);

	m_genderCombo.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CRegBasicDlg::OnWizardNext() 
{
	if (!UpdateData())
		return -1;

	if (m_passwd.IsEmpty() || m_nick.IsEmpty()) {
		myMessageBox(IDS_BASICINFO_MUST_FILL, IDS_ERROR, this);
		return -1;
	}
	if (m_passwd.Compare(m_passwdAgain) != 0) {
		myMessageBox(IDS_PASSWD_MISMATCH, IDS_ERROR, this);
		return -1;
	}
	
	return CPropertyPage::OnWizardNext();
}
