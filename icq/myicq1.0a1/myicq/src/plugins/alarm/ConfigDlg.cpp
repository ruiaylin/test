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

// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "alarm.h"
#include "ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog


CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
	m_enable = FALSE;
	m_text = _T("");
	m_time = 0;
	m_date = 0;
	//}}AFX_DATA_INIT
}


void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	DDX_Check(pDX, IDC_ENABLE, m_enable);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_DateTimeCtrl(pDX, IDC_TIME, m_time);
	DDX_DateTimeCtrl(pDX, IDC_DATE, m_date);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers
