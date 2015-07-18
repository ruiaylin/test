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

// MsgSearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "MsgSearchDlg.h"
#include "icqdb.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgSearchDlg dialog


CMsgSearchDlg::CMsgSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsgSearchDlg)
	m_searchMode = 0;
	m_searchText = _T("");
	//}}AFX_DATA_INIT

	Create(IDD, pParent);
}

void CMsgSearchDlg::searchMsg(QID &qid, CString &nick)
{
	PtrList l;
	IcqDB::loadMsg(&qid, l);

	CString str;
	while (!l.empty()) {
		IcqMsg *msg = (IcqMsg *) l.front();
		l.pop_front();

		int n = m_searchResult.GetItemCount();
		CString text(msg->text.c_str());
		if (text.Find(m_searchText) >= 0) {
			m_searchResult.InsertItem(n, nick);
			m_searchResult.SetItemText(n, 1, text);
		}
		delete msg;
	}
}

void CMsgSearchDlg::onSearchMsg(QID &qid, CString &nick)
{
/*	if (qid.uin)
		searchMsg(qid, nick);
	else {
		int n = m_contactCombo.GetCount();
		for (int i = 1; i < n; ++i) {
			uint32 uin = m_contactCombo.GetItemData(i);
			m_contactCombo.GetLBText(i, nick);
			searchMsg(uin, nick);
		}
	}*/
}

void CMsgSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgSearchDlg)
	DDX_Control(pDX, IDC_RESULT, m_searchResult);
	DDX_Control(pDX, IDC_CONTACT_COMBO, m_contactCombo);
	DDX_Radio(pDX, IDC_MODE, m_searchMode);
	DDX_Text(pDX, IDC_SEARCH_TEXT, m_searchText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgSearchDlg, CDialog)
	//{{AFX_MSG_MAP(CMsgSearchDlg)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgSearchDlg message handlers

BOOL CMsgSearchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString str;
/*
	UinList l;
	IcqDB::getMsgQIDList(l);
	UinList::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		string s = *it;
		IcqContact *c = icqLink->findContact(uin);
		if (c)
			str.Format("%lu(%s)", c->uin, c->nick.c_str());
		else
			str.Format("%lu", uin);

		int i = m_contactCombo.AddString(str);
		m_contactCombo.SetItemData(i, uin);
	}
*/
	m_contactCombo.SetCurSel(0);

	m_searchResult.SetExtendedStyle(
		m_searchResult.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	
	str.LoadString(IDS_LOCATION);
	m_searchResult.InsertColumn(0, str, LVCFMT_LEFT, 100);
	str.LoadString(IDS_CONTENT);
	m_searchResult.InsertColumn(1, str, LVCFMT_LEFT, 300);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMsgSearchDlg::OnSearch() 
{
	m_searchResult.DeleteAllItems();
	
	UpdateData();

/*	int i = m_contactCombo.GetCurSel();
	if (i >= 0) {
		uint32 uin = m_contactCombo.GetItemData(i);
		CString nick;
		m_contactCombo.GetLBText(i, nick);

		if (m_searchMode == 0)
			onSearchMsg(uin, nick);
	}*/
}

void CMsgSearchDlg::PostNcDestroy() 
{
	delete this;
}
