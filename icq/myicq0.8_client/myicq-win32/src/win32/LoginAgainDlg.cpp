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

// LoginAgainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "LoginAgainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginAgainDlg dialog


CLoginAgainDlg::CLoginAgainDlg(DWORD uin, CWnd* pParent /*=NULL*/)
	: CMyDlg(CLoginAgainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginAgainDlg)
	m_passwd = _T("");
	m_uin = _T("");
	//}}AFX_DATA_INIT
	m_uin.Format("%lu", uin);
}


void CLoginAgainDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginAgainDlg)
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
	DDX_Text(pDX, IDC_UIN, m_uin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginAgainDlg, CMyDlg)
	//{{AFX_MSG_MAP(CLoginAgainDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginAgainDlg message handlers
