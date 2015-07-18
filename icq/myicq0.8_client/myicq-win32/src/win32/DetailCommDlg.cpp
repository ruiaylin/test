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

// DetailCommDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "DetailCommDlg.h"
#include "icqclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDetailCommDlg property page

IMPLEMENT_DYNCREATE(CDetailCommDlg, CPropertyPage)

CDetailCommDlg::CDetailCommDlg() : CPropertyPage(CDetailCommDlg::IDD)
{
	//{{AFX_DATA_INIT(CDetailCommDlg)
	m_address = _T("");
	m_email = _T("");
	m_tel = _T("");
	m_zipcode = _T("");
	//}}AFX_DATA_INIT
}

CDetailCommDlg::~CDetailCommDlg()
{
}

void CDetailCommDlg::enableAll(BOOL enable)
{
	((CEdit *) GetDlgItem(IDC_EMAIL))->SetReadOnly(!enable);
	((CEdit *) GetDlgItem(IDC_ADDRESS))->SetReadOnly(!enable);
	((CEdit *) GetDlgItem(IDC_ZIPCODE))->SetReadOnly(!enable);
	((CEdit *) GetDlgItem(IDC_TEL))->SetReadOnly(!enable);
	GetDlgItem(IDC_ACCESS_PUBLIC)->EnableWindow(enable);
	GetDlgItem(IDC_ACCESS_FRIEND)->EnableWindow(enable);
	GetDlgItem(IDC_ACCESS_PRIVATE)->EnableWindow(enable);
}

void CDetailCommDlg::setData(IcqInfo *info)
{
	m_email = info->email.c_str();
	m_address = info->address.c_str();
	m_zipcode = info->zipcode.c_str();
	m_tel = info->tel.c_str();
	UpdateData(FALSE);
}

void CDetailCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDetailCommDlg)
	DDX_Text(pDX, IDC_ADDRESS, m_address);
	DDX_Text(pDX, IDC_EMAIL, m_email);
	DDX_Text(pDX, IDC_TEL, m_tel);
	DDX_Text(pDX, IDC_ZIPCODE, m_zipcode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDetailCommDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CDetailCommDlg)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetailCommDlg message handlers

void CDetailCommDlg::OnCancel() 
{
	GetParent()->DestroyWindow();
}

void CDetailCommDlg::OnUpdate() 
{
	GetParent()->SendMessage(WM_COMMAND, IDC_UPDATE);
}
