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

// SearchUINDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SearchUINDlg.h"
#include "SearchWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchUINDlg property page

IMPLEMENT_DYNCREATE(CSearchUINDlg, CPropertyPage)

CSearchUINDlg::CSearchUINDlg() : CPropertyPage(CSearchUINDlg::IDD)
{
	//{{AFX_DATA_INIT(CSearchUINDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSearchUINDlg::~CSearchUINDlg()
{
}

void CSearchUINDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchUINDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchUINDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSearchUINDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchUINDlg message handlers

BOOL CSearchUINDlg::OnSetActive() 
{
	CPropertySheet *wiz = (CPropertySheet *) GetParent();
	wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}

LRESULT CSearchUINDlg::OnWizardNext() 
{
	CString str;
	GetDlgItemText(IDC_UIN, str);
	if (!m_qid.parse(str)) {
		myMessageBox(IDS_ERROR_FORMAT_UIN, IDS_ERROR, this, MB_OK);
		return -1;
	}

	((CSearchWizard *) GetParent())->qid = m_qid;

	return IDD_SEARCH_RESULT;
}
