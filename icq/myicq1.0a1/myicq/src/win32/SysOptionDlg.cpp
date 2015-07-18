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

// SysOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SysOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysOptionDlg

IMPLEMENT_DYNAMIC(CSysOptionDlg, CPropertySheet)

CSysOptionDlg::CSysOptionDlg(CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(IDS_SYS_OPTION, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_USEHICON | PSH_NOAPPLYNOW;
	m_psh.hIcon = getApp()->m_hIcon;	
	
	AddPage(&generalDlg);
	AddPage(&replyDlg);
	AddPage(&networkDlg);
	AddPage(&soundDlg);
	AddPage(&pluginDlg);
}

CSysOptionDlg::~CSysOptionDlg()
{
}


BEGIN_MESSAGE_MAP(CSysOptionDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CSysOptionDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysOptionDlg message handlers
