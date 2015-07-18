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

// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "LoginDlg.h"
#include "icqdb.h"
#include "icqconfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog


CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CMyDlg(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_invis = FALSE;
	m_passwd = _T("");
	m_storePasswd = FALSE;
	//}}AFX_DATA_INIT
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Control(pDX, IDC_USER, m_cmbUser);
	DDX_Check(pDX, IDC_INVISIBLE, m_invis);
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
	DDX_Check(pDX, IDC_STOREPASSWD, m_storePasswd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CMyDlg)
	//{{AFX_MSG_MAP(CLoginDlg)
	ON_BN_CLICKED(IDC_REGWIZARD, OnRegWizard)
	ON_CBN_SELCHANGE(IDC_USER, OnSelchangeUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

void CLoginDlg::OnRegWizard() 
{
	EndDialog(IDC_REGWIZARD);
}

BOOL CLoginDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	QIDList l;
	IcqConfig::getAllUsers(l);
	if (l.size() <= 0)
		OnRegWizard();
	else {
		QIDList::iterator it;
		for (it = l.begin(); it != l.end(); ++it)
			m_cmbUser.AddString((*it).toString());

		m_cmbUser.SetCurSel(0);
		OnSelchangeUser();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoginDlg::OnOK() 
{
	BOOL invis = options.flags.test(UF_LOGIN_INVIS);
	BOOL storePasswd = options.flags.test(UF_STORE_PASSWD);
	UpdateData();
	if (m_invis != invis || m_storePasswd != storePasswd) {
		options.flags.set(UF_LOGIN_INVIS, m_invis);
		options.flags.set(UF_STORE_PASSWD, m_storePasswd);
		IcqDB::saveOptions(options);
	}

	QID qid;
	CString str;
	int i = m_cmbUser.GetCurSel();
	m_cmbUser.GetLBText(i, str);
	qid.parse(str);
	IcqConfig::addUser(qid);
	
	CMyDlg::OnOK();
}

void CLoginDlg::OnSelchangeUser() 
{
	int i = m_cmbUser.GetCurSel();
	CString str;
	m_cmbUser.GetLBText(i, str);
	m_qid.parse(str);

	getApp()->getUserDir(str, m_qid);
	IcqDB::setDir(str);

	if (IcqDB::loadOptions(options)) {
		m_invis = options.flags.test(UF_LOGIN_INVIS);
		m_storePasswd = options.flags.test(UF_STORE_PASSWD);
		if (m_storePasswd) {
			IcqUser user;
			user.qid = m_qid;
			IcqDB::loadUser(user);
			m_passwd = user.passwd.c_str();
		} else
			m_passwd.Empty();
		UpdateData(FALSE);
	}
}
