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

// SearchServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SearchServerDlg.h"
#include "SearchWizard.h"
#include "udpsession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchServerDlg property page

IMPLEMENT_DYNCREATE(CSearchServerDlg, CPropertyPage)

CSearchServerDlg::CSearchServerDlg() : CPropertyPage(CSearchServerDlg::IDD)
{
	//{{AFX_DATA_INIT(CSearchServerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSearchServerDlg::~CSearchServerDlg()
{
}

void CSearchServerDlg::getServer(CString &name)
{
	name.Empty();
	
	POSITION pos = m_serverList.GetFirstSelectedItemPosition();
	if (pos) {
		int i = m_serverList.GetNextSelectedItem(pos);
		name = m_serverList.GetItemText(i, 0);
	}
}

void CSearchServerDlg::onServerListReply(SERVER_INFO info[], int n)
{
	for (int i = 0; i < n; i++) {
		CString str;
		str.Format("%lu", info[i].sessionCount);

		m_serverList.InsertItem(i, info[i].domain.c_str());
		m_serverList.SetItemText(i, 1, str);
		m_serverList.SetItemText(i, 2, info[i].desc.c_str());
	}

	CString str;
	str.LoadString(IDS_FINISHED);
	SetDlgItemText(IDC_STATUS, str);
}

void CSearchServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchServerDlg)
	DDX_Control(pDX, IDC_SERVER, m_serverList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchServerDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSearchServerDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchServerDlg message handlers

BOOL CSearchServerDlg::OnSetActive() 
{
	CPropertySheet *wiz = (CPropertySheet *) GetParent();
	wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}

BOOL CSearchServerDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_serverList.SetExtendedStyle(m_serverList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	CString str;
	str.LoadString(IDS_SERVER);
	m_serverList.InsertColumn(0, str, LVCFMT_LEFT, 120);
	str.LoadString(IDS_NUM_PEOPLE);
	m_serverList.InsertColumn(1, str,LVCFMT_LEFT, 80);
	str.LoadString(IDS_DESC);
	m_serverList.InsertColumn(2, str, LVCFMT_LEFT, 240);

	// Get server list
	getUdpSession()->getServerList();

	str.LoadString(IDS_FETCHING_SERVER_LIST);
	SetDlgItemText(IDC_STATUS, str);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CSearchServerDlg::OnWizardBack() 
{
	return IDD_SEARCH_MODE;
}

LRESULT CSearchServerDlg::OnWizardNext() 
{
	CString domain;
	getServer(domain);
	if (domain.IsEmpty())
		return -1;

	((CSearchWizard *) GetParent())->qid.domain = domain;

	return CPropertyPage::OnWizardNext();
}
