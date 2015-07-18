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

// SearchModeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SearchModeDlg.h"
#include "SearchWizard.h"
#include "serversession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSearchModeDlg property page

IMPLEMENT_DYNCREATE(CSearchModeDlg, CPropertyPage)

CSearchModeDlg::CSearchModeDlg() : CPropertyPage(CSearchModeDlg::IDD)
{
	//{{AFX_DATA_INIT(CSearchModeDlg)
	m_mode = 0;
	m_nrOnlines = _T("");
	//}}AFX_DATA_INIT

	uint32 n = serverSession()->sessionCount;
	if (n)
		m_nrOnlines.Format("%lu", n);
	else
		m_nrOnlines.LoadString(IDS_UNKNOWN);
}

CSearchModeDlg::~CSearchModeDlg()
{
}

void CSearchModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchModeDlg)
	DDX_Radio(pDX, IDC_SEARCH_ONLINE, m_mode);
	DDX_Text(pDX, IDC_NR_ONLINES, m_nrOnlines);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchModeDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSearchModeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchModeDlg message handlers

LRESULT CSearchModeDlg::OnWizardNext() 
{
	UpdateData();
	if (m_mode == SEARCH_MODE_ONLINE)
		return IDD_SEARCH_RESULT;
	else
		return CPropertyPage::OnWizardNext();
}

BOOL CSearchModeDlg::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}

void CSearchModeDlg::OnCancel() 
{
	GetParent()->DestroyWindow();
}
