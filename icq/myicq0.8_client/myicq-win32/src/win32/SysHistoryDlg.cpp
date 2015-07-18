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

// SysHistoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SysHistoryDlg.h"
#include "SysMsgDlg.h"
#include "icqdb.h"
#include "icqclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysHistoryDlg dialog


CSysHistoryDlg::CSysHistoryDlg(CWnd* pParent /*=NULL*/)
:CMyDlg(CSysHistoryDlg::IDD, pParent), IcqWindow(WIN_SYS_HISTORY)
{
	//{{AFX_DATA_INIT(CSysHistoryDlg)
	//}}AFX_DATA_INIT
}

void CSysHistoryDlg::deleteAllItems()
{
	for (int i = m_ctlHistory.GetItemCount() - 1; i >= 0; i--) {
		IcqMsg *msg = (IcqMsg *) m_ctlHistory.GetItemData(i);
		delete msg;
	}
	m_ctlHistory.DeleteAllItems();
}

void CSysHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysHistoryDlg)
	DDX_Control(pDX, IDC_HISTORY, m_ctlHistory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysHistoryDlg, CMyDlg)
	//{{AFX_MSG_MAP(CSysHistoryDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_HISTORY, OnDblclkHistory)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DELETE_ALL, OnDeleteAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysHistoryDlg message handlers

void CSysHistoryDlg::OnCancel() 
{
	deleteAllItems();	
	DestroyWindow();
}

void CSysHistoryDlg::PostNcDestroy() 
{
	delete this;
}

BOOL CSysHistoryDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	m_ctlHistory.SetFullRowSel(TRUE);
	
	CString str;
	str.LoadString(IDS_FROM_WHERE);
	m_ctlHistory.InsertColumn(0, str, LVCFMT_LEFT, 80);
	str.LoadString(IDS_DATE);
	m_ctlHistory.InsertColumn(1, str, LVCFMT_LEFT, 90);
	str.LoadString(IDS_TIME);
	m_ctlHistory.InsertColumn(2, str, LVCFMT_LEFT, 80);
	str.LoadString(IDS_CONTENT);
	m_ctlHistory.InsertColumn(3, str, LVCFMT_LEFT, 140);

	PtrList l;
	IcqDB::loadMsg(0, l);

	for (int i = 0; !l.empty(); i++) {
		IcqMsg *msg = (IcqMsg *) l.back();
		l.pop_back();

		str.Format("%lu", msg->uin);
		m_ctlHistory.InsertItem(i, str);
		m_ctlHistory.SetItemData(i, (DWORD) msg);

		CTime t(msg->when);
		m_ctlHistory.SetItemText(i, 1, t.Format("%Y-%m-%d"));
		m_ctlHistory.SetItemText(i, 2, t.Format("%H:%M:%S"));
		getMsgText(msg, str);
		m_ctlHistory.SetItemText(i, 3, str);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSysHistoryDlg::OnDblclkHistory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	
	POSITION pos = m_ctlHistory.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;

	int i = m_ctlHistory.GetNextSelectedItem(pos);
	IcqMsg *msg = (IcqMsg *) m_ctlHistory.GetItemData(i);

	CSysMsgDlg *win = new CSysMsgDlg(msg, FALSE);
	win->Create(IDD_SYS_MESSAGE);
}

void CSysHistoryDlg::OnSize(UINT nType, int cx, int cy) 
{
	CMyDlg::OnSize(nType, cx, cy);
	
	if (!m_ctlHistory)
		return;

	int y = cy - 30;
	m_ctlHistory.MoveWindow(0, 0, cx, y);
	y += 5;
	CRect rc;

	CWnd *pWnd = GetDlgItem(IDC_DELETE);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	rc.bottom = y + rc.Height();
	rc.top = y;
	pWnd->MoveWindow(rc);

	pWnd = GetDlgItem(IDC_DELETE_ALL);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	rc.bottom = y + rc.Height();
	rc.top = y;
	pWnd->MoveWindow(rc);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(rc);
	rc.bottom = y + rc.Height();
	rc.top = y;
	rc.left = cx - 5 - rc.Width();
	rc.right = cx - 5;
	pWnd->MoveWindow(rc);
}

void CSysHistoryDlg::OnDelete() 
{
	POSITION pos = m_ctlHistory.GetFirstSelectedItemPosition();
	if (pos == NULL) {
		myMessageBox(IDS_SELECT_RECORD, IDS_ERROR, this);
		return;
	}

	int i = m_ctlHistory.GetNextSelectedItem(pos);
	IcqMsg *msg = (IcqMsg *) m_ctlHistory.GetItemData(i);
	delete msg;
	m_ctlHistory.DeleteItem(i);

	IcqDB::delMsg(0, m_ctlHistory.GetItemCount() - i);
}

void CSysHistoryDlg::OnDeleteAll() 
{
	CString strText, strCaption;
	strText.LoadString(IDS_PROMPT_DEL_SYSMSG);
	strCaption.LoadString(IDS_WARNING);
	if (myMessageBox(IDS_PROMPT_DEL_SYSMSG, IDS_WARNING, this,
			MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
	
	deleteAllItems();
	IcqDB::delMsg(0);
}
