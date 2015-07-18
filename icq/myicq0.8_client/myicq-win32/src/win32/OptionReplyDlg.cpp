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

// OptionReplyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "OptionReplyDlg.h"
#include "icqlink.h"
#include "icqdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionReplyDlg property page

IMPLEMENT_DYNCREATE(COptionReplyDlg, CPropertyPage)

COptionReplyDlg::COptionReplyDlg() : CPropertyPage(COptionReplyDlg::IDD)
{
	//{{AFX_DATA_INIT(COptionReplyDlg)
	//}}AFX_DATA_INIT
	curAutoReply = -1;
	curQuickReply = -1;

	IcqOption &options = icqLink->options;
	
	m_autoSwitchStatus = options.flags.test(UF_AUTO_SWITCH_STATUS);
	m_autoCancelAway = options.flags.test(UF_AUTO_CANCEL_AWAY);
	switch (options.autoStatus) {
	case STATUS_AWAY:
		m_autoStatus = 0;
		break;
	case STATUS_INVIS:
		m_autoStatus = 1;
		break;
	case STATUS_OFFLINE:
		m_autoStatus = 2;
		break;
	}
	m_autoStatusTime = options.autoStatusTime;
}

COptionReplyDlg::~COptionReplyDlg()
{
}

void COptionReplyDlg::enableControls(BOOL enable)
{
	GetDlgItem(IDC_CHOOSER_AUTO_REPLY)->EnableWindow(enable);
	GetDlgItem(IDC_TEXT_AUTO_REPLY)->EnableWindow(enable && m_cmbAutoReply.GetCount());
}

void COptionReplyDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionReplyDlg)
	DDX_Control(pDX, IDC_CHOOSER_QUICK_REPLY, m_cmbQuickReply);
	DDX_Control(pDX, IDC_CHOOSER_AUTO_REPLY, m_cmbAutoReply);
	DDX_Check(pDX, IDC_AUTO_SWITCH_STATUS, m_autoSwitchStatus);
	DDX_Check(pDX, IDC_AUTO_CANCEL_AWAY, m_autoCancelAway);
	DDX_Text(pDX, IDC_AUTO_STATUS_TIME, m_autoStatusTime);
	DDX_CBIndex(pDX, IDC_SWITCH_STATUS, m_autoStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionReplyDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptionReplyDlg)
	ON_BN_CLICKED(IDC_AUTO_REPLY, OnAutoReply)
	ON_BN_CLICKED(IDC_ADD_AUTO_REPLY, OnAddAutoReply)
	ON_BN_CLICKED(IDC_DEL_AUTO_REPLY, OnDelAutoReply)
	ON_CBN_SELCHANGE(IDC_CHOOSER_AUTO_REPLY, OnSelchangeAutoReply)
	ON_BN_CLICKED(IDC_ADD_QUICK_REPLY, OnAddQuickReply)
	ON_BN_CLICKED(IDC_DEL_QUICK_REPLY, OnDelQuickReply)
	ON_CBN_SELCHANGE(IDC_CHOOSER_QUICK_REPLY, OnSelchangeQuickReply)
	ON_BN_CLICKED(IDC_AUTO_SWITCH_STATUS, OnAutoSwitchStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionReplyDlg message handlers

void COptionReplyDlg::OnAutoReply() 
{
	enableControls(IsDlgButtonChecked(IDC_AUTO_REPLY));
}

BOOL COptionReplyDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	OnAutoSwitchStatus();

	IcqOption &options = icqLink->options;
	int sel = -1;
	int i = 0;

	StrList l;
	IcqDB::loadAutoReply(l);
	StrList::iterator it;
	for (it = l.begin(); it != l.end(); ++it, ++i) {
		CString str = (*it).c_str();
		if (sel < 0 && str == options.autoReplyText.c_str())
			sel = i;
		autoReplyList.AddTail(str);
		str.Format("%d", i + 1);
		m_cmbAutoReply.AddString(str);
	}
	m_cmbAutoReply.SetCurSel(sel >= 0 ? sel : 0);
	OnSelchangeAutoReply();

	l.clear();
	IcqDB::loadQuickReply(l);
	i = 0;
	sel = -1;
	for (it = l.begin(); it != l.end(); ++it, ++i) {
		CString str = (*it).c_str();
		if (sel < 0 && str == options.quickReplyText.c_str())
			sel = i;
		quickReplyList.AddTail(str);
		str.Format("%d", i + 1);
		m_cmbQuickReply.AddString(str);
	}
	m_cmbQuickReply.SetCurSel(sel >= 0 ? sel : 0);
	OnSelchangeQuickReply();

	bool b = options.flags.test(UF_AUTO_REPLY);
	CheckDlgButton(IDC_AUTO_REPLY, b);
	enableControls(b);

	GetDlgItem(IDC_TEXT_QUICK_REPLY)->EnableWindow(m_cmbQuickReply.GetCount() > 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionReplyDlg::OnAddAutoReply() 
{
	if (autoReplyList.IsEmpty()) {
		GetDlgItem(IDC_DEL_AUTO_REPLY)->EnableWindow();
		GetDlgItem(IDC_TEXT_AUTO_REPLY)->EnableWindow();
	}
	autoReplyList.AddTail("");

	int n = m_cmbAutoReply.GetCount();
	CString str;
	str.Format("%d", n + 1);
	int i = m_cmbAutoReply.AddString(str);
	m_cmbAutoReply.SetCurSel(i);
	OnSelchangeAutoReply();
}

void COptionReplyDlg::OnDelAutoReply() 
{
	POSITION pos = autoReplyList.FindIndex(curAutoReply);
	if (pos)
		autoReplyList.RemoveAt(pos);

	if (autoReplyList.IsEmpty()) {
		GetDlgItem(IDC_DEL_AUTO_REPLY)->EnableWindow(FALSE);
		GetDlgItem(IDC_TEXT_AUTO_REPLY)->EnableWindow(FALSE);
	}

	int n = m_cmbAutoReply.GetCount();
	m_cmbAutoReply.DeleteString(n - 1);
	if (curAutoReply > 0)
		m_cmbAutoReply.SetCurSel(curAutoReply - 1);
	else if (--n <= 0)
		m_cmbAutoReply.SetCurSel(-1);
	curAutoReply = -1;
	OnSelchangeAutoReply();
}

void COptionReplyDlg::OnSelchangeAutoReply() 
{
	POSITION pos = autoReplyList.FindIndex(curAutoReply);
	if (pos)
		GetDlgItemText(IDC_TEXT_AUTO_REPLY, autoReplyList.GetAt(pos));

	curAutoReply = m_cmbAutoReply.GetCurSel();
	pos = autoReplyList.FindIndex(curAutoReply);
	SetDlgItemText(IDC_TEXT_AUTO_REPLY, pos ? autoReplyList.GetAt(pos) : "");
}

void COptionReplyDlg::OnAddQuickReply() 
{
	if (quickReplyList.IsEmpty()) {
		GetDlgItem(IDC_DEL_QUICK_REPLY)->EnableWindow();
		GetDlgItem(IDC_TEXT_QUICK_REPLY)->EnableWindow();
	}
	quickReplyList.AddTail("");
	
	int n = m_cmbQuickReply.GetCount();
	CString str;
	str.Format("%d", n + 1);
	int i = m_cmbQuickReply.AddString(str);
	m_cmbQuickReply.SetCurSel(i);
	OnSelchangeQuickReply();
}

void COptionReplyDlg::OnDelQuickReply() 
{
	POSITION pos = quickReplyList.FindIndex(curQuickReply);
	if (pos)
		quickReplyList.RemoveAt(pos);

	if (quickReplyList.IsEmpty()) {
		GetDlgItem(IDC_DEL_QUICK_REPLY)->EnableWindow(FALSE);
		GetDlgItem(IDC_TEXT_QUICK_REPLY)->EnableWindow(FALSE);
	}

	int n = m_cmbQuickReply.GetCount();
	m_cmbQuickReply.DeleteString(n - 1);
	if (curQuickReply > 0)
		m_cmbQuickReply.SetCurSel(curQuickReply - 1);
	else if (--n <= 0)
		m_cmbQuickReply.SetCurSel(-1);
	curQuickReply = -1;
	OnSelchangeQuickReply();	
}

void COptionReplyDlg::OnSelchangeQuickReply() 
{
	POSITION pos = quickReplyList.FindIndex(curQuickReply);
	if (pos)
		GetDlgItemText(IDC_TEXT_QUICK_REPLY, quickReplyList.GetAt(pos));

	curQuickReply = m_cmbQuickReply.GetCurSel();
	pos = quickReplyList.FindIndex(curQuickReply);
	SetDlgItemText(IDC_TEXT_QUICK_REPLY, pos ? autoReplyList.GetAt(pos) : "");	
}

void COptionReplyDlg::OnOK() 
{
	OnSelchangeAutoReply();
	OnSelchangeQuickReply();
	
	StrList l;
	POSITION pos = autoReplyList.GetHeadPosition();
	while (pos)
		l.push_back((LPCTSTR) autoReplyList.GetNext(pos));
	IcqDB::saveAutoReply(l);

	l.clear();
	pos = quickReplyList.GetHeadPosition();
	while (pos)
		l.push_back((LPCTSTR) quickReplyList.GetNext(pos));
	IcqDB::saveQuickReply(l);

	IcqOption &options = icqLink->options;

	options.flags.set(UF_AUTO_REPLY, IsDlgButtonChecked(IDC_AUTO_REPLY));
	CString str;
	GetDlgItemText(IDC_TEXT_AUTO_REPLY, str);
	options.autoReplyText = str;
	GetDlgItemText(IDC_TEXT_QUICK_REPLY, str);
	options.quickReplyText = str;

	options.flags.set(UF_AUTO_SWITCH_STATUS, m_autoSwitchStatus);
	options.flags.set(UF_AUTO_CANCEL_AWAY, m_autoCancelAway);
	switch (m_autoStatus) {
	case 0:
		options.autoStatus = STATUS_AWAY;
		break;
	case 1:
		options.autoStatus = STATUS_INVIS;
		break;
	case 2:
		options.autoStatus = STATUS_OFFLINE;
		break;
	}
	options.autoStatusTime = m_autoStatusTime;

	CPropertyPage::OnOK();
}

void COptionReplyDlg::OnAutoSwitchStatus() 
{
	BOOL b = IsDlgButtonChecked(IDC_AUTO_SWITCH_STATUS);

	GetDlgItem(IDC_AUTO_CANCEL_AWAY)->EnableWindow(b);
	GetDlgItem(IDC_AUTO_STATUS_TIME)->EnableWindow(b);
	GetDlgItem(IDC_SWITCH_STATUS)->EnableWindow(b);
}
