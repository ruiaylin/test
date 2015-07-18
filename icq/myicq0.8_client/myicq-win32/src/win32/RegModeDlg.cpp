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

// RegModeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RegModeDlg.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegModeDlg property page

IMPLEMENT_DYNCREATE(CRegModeDlg, CPropertyPage)

CRegModeDlg::CRegModeDlg() : CPropertyPage(CRegModeDlg::IDD)
{
	//{{AFX_DATA_INIT(CRegModeDlg)
	m_mode = 0;
	m_uin = 0;
	m_passwd = _T("");
	//}}AFX_DATA_INIT

	m_uin = icqLink->myInfo.uin;
	m_passwd = icqLink->myInfo.passwd.c_str();
}

CRegModeDlg::~CRegModeDlg()
{
}

void CRegModeDlg::enableControls(BOOL enable)
{
	GetDlgItem(IDC_STATIC_UIN)->EnableWindow(enable);
	GetDlgItem(IDC_UIN)->EnableWindow(enable);
	GetDlgItem(IDC_STATIC_PASSWD)->EnableWindow(enable);
	GetDlgItem(IDC_PASSWD)->EnableWindow(enable);
}

void CRegModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegModeDlg)
	DDX_Radio(pDX, IDC_NEW_UIN, m_mode);
	DDX_Text(pDX, IDC_UIN, m_uin);
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegModeDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CRegModeDlg)
	ON_BN_CLICKED(IDC_NEW_UIN, OnNewUin)
	ON_BN_CLICKED(IDC_EXISTING_UIN, OnExistingUin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegModeDlg message handlers

BOOL CRegModeDlg::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}

void CRegModeDlg::OnNewUin() 
{
	enableControls(FALSE);
}

void CRegModeDlg::OnExistingUin() 
{
	enableControls(TRUE);
}

LRESULT CRegModeDlg::OnWizardNext() 
{
	UpdateData();
	if (m_mode == 1)
		return IDD_REG_NETWORK;
	return CPropertyPage::OnWizardNext();
}
