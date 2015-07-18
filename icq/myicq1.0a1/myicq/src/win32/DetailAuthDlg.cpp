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

// DetailAuthDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "DetailAuthDlg.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDetailAuthDlg property page

IMPLEMENT_DYNCREATE(CDetailAuthDlg, CPropertyPage)

CDetailAuthDlg::CDetailAuthDlg() : CPropertyPage(CDetailAuthDlg::IDD)
{
	//{{AFX_DATA_INIT(CDetailAuthDlg)
	m_passwd = _T("");
	m_passwdAgain = _T("");
	m_passwdOld = _T("");
	m_auth = 0;
	m_modifyPasswd = FALSE;
	//}}AFX_DATA_INIT
}

CDetailAuthDlg::~CDetailAuthDlg()
{
}

void CDetailAuthDlg::enablePasswd(BOOL enable)
{
	GetDlgItem(IDC_PASSWD_OLD)->EnableWindow(enable);
	GetDlgItem(IDC_PASSWD)->EnableWindow(enable);
	GetDlgItem(IDC_PASSWD_AGAIN)->EnableWindow(enable);
}

void CDetailAuthDlg::enableAll(BOOL enable)
{
	GetDlgItem(IDC_MODIFY_PASSWD)->EnableWindow(enable);
	GetDlgItem(IDC_AUTH_ALL)->EnableWindow(enable);
	GetDlgItem(IDC_AUTH_REQUEST)->EnableWindow(enable);
	GetDlgItem(IDC_AUTH_NONE)->EnableWindow(enable);
	enablePasswd(enable && IsDlgButtonChecked(IDC_MODIFY_PASSWD) == 1);
}

void CDetailAuthDlg::setData(IcqUser *user)
{
	m_auth = user->auth;
	UpdateData(FALSE);
}

BOOL CDetailAuthDlg::UpdateData(BOOL bSaveAndValidate)
{
	if (CDialog::UpdateData(bSaveAndValidate)) {
		if (bSaveAndValidate && m_modifyPasswd) {
			if (m_passwdOld != icqLink->myInfo.passwd.c_str()) {
				myMessageBox(IDS_WRONG_PASSWD, IDS_ERROR, this, MB_ICONERROR | MB_OK);
				return FALSE;
			} else if (m_passwd != m_passwdAgain) {
				myMessageBox(IDS_PASSWD_MISMATCH, IDS_ERROR, this, MB_ICONERROR | MB_OK);
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CDetailAuthDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDetailAuthDlg)
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
	DDX_Text(pDX, IDC_PASSWD_AGAIN, m_passwdAgain);
	DDX_Text(pDX, IDC_PASSWD_OLD, m_passwdOld);
	DDX_Radio(pDX, IDC_AUTH_ALL, m_auth);
	DDX_Check(pDX, IDC_MODIFY_PASSWD, m_modifyPasswd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDetailAuthDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CDetailAuthDlg)
	ON_BN_CLICKED(IDC_MODIFY_PASSWD, OnModifyPasswd)
	ON_BN_CLICKED(IDC_UPDATE, OnModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetailAuthDlg message handlers

void CDetailAuthDlg::OnModifyPasswd() 
{
	enablePasswd(IsDlgButtonChecked(IDC_MODIFY_PASSWD) == 1);
}

void CDetailAuthDlg::OnModify() 
{
	GetParent()->SendMessage(WM_COMMAND, IDC_UPDATE);
}

void CDetailAuthDlg::OnCancel() 
{
	GetParent()->DestroyWindow();
}
