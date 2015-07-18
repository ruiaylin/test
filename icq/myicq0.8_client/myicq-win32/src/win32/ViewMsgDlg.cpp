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

// ViewMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "ViewMsgDlg.h"
#include "ViewDetailDlg.h"
#include "SendMsgDlg.h"
#include "myicqDlg.h"
#include "icqdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewMsgDlg dialog


CViewMsgDlg::CViewMsgDlg(IcqContact *contact, CWnd* pParent /*=NULL*/)
:CMyDlg(CViewMsgDlg::IDD, pParent), IcqWindow(WIN_VIEW_MESSAGE, contact->uin)
{
	//{{AFX_DATA_INIT(CViewMsgDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	this->contact = contact;
	expanded = FALSE;
}

CSendMsgDlg *CViewMsgDlg::createSendMsgDlg()
{
	CSendMsgDlg *pDlg;
	IcqWindow *win = icqLink->findWindow(WIN_SEND_MESSAGE, uin);
	if (!win) {
		pDlg = new CSendMsgDlg(contact);
		pDlg->Create(IDD_SEND_MESSAGE);

		CRect rc;
		GetWindowRect(rc);
		pDlg->SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	} else {
		pDlg = (CSendMsgDlg *) win;
		pDlg->ShowWindow(SW_NORMAL);
		pDlg->BringWindowToTop();
	}
	return pDlg;
}

void CViewMsgDlg::expand()
{
	SetWindowPos(NULL, 0, 0, wholeSize.cx, wholeSize.cy, SWP_NOZORDER | SWP_NOMOVE);
	expanded = TRUE;
}

void CViewMsgDlg::shrink()
{
	CRect rc;
	GetDlgItem(IDC_LOWER)->GetWindowRect(rc);
	SetWindowPos(NULL, 0, 0, wholeSize.cx, wholeSize.cy - rc.Height(),
		SWP_NOZORDER | SWP_NOMOVE);
	expanded = FALSE;
}

void CViewMsgDlg::onRecvMessage()
{
	GetDlgItem(IDC_NEXT_MSG)->EnableWindow();
}

void CViewMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewMsgDlg)
	DDX_Control(pDX, IDC_MSG_HISTORY, m_lstHistory);
	DDX_Control(pDX, IDC_PIC, m_btnPic);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CViewMsgDlg, CMyDlg)
	//{{AFX_MSG_MAP(CViewMsgDlg)
	ON_BN_CLICKED(IDC_NEXT_MSG, OnNextMsg)
	ON_BN_CLICKED(IDC_MENU_QUICK_REPLY, OnMenuQuickReply)
	ON_BN_CLICKED(IDC_QUICK_REPLY, OnQuickReply)
	ON_BN_CLICKED(IDC_PIC, OnPic)
	ON_BN_CLICKED(IDC_HISTORY, OnHistory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewMsgDlg message handlers

BOOL CViewMsgDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	int x = contact->winX;
	int y = contact->winY;
	if (x != -1 || y != -1)
		SetWindowPos(NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	CRect rc;
	GetWindowRect(rc);
	wholeSize = rc.Size();

	shrink();

	SetDlgItemText(IDC_NICK, contact->nick.c_str());
	SetDlgItemInt(IDC_UIN, contact->uin, FALSE);
	SetDlgItemText(IDC_EMAIL, contact->email.c_str());
	m_btnPic.SetIcon(getApp()->largeImageList.ExtractIcon(contact->face));

	in_addr addr;
	addr.s_addr = htonl(contact->ip);
	SetDlgItemText(IDC_ADDR, inet_ntoa(addr));
	SetDlgItemInt(IDC_PORT, contact->port, FALSE);

	OnNextMsg();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewMsgDlg::OnNextMsg() 
{
	IcqMsg *msg = ((CIcqDlg *) AfxGetMainWnd())->fetchMsg(uin);

	if (msg->type == MSG_TCP_REQUEST) {
/*		CSendFileDlg *win = new CSendFileDlg(msg);
		win->Create(IDD_SEND_FILE);*/
	} else {
		CTime t(msg->when);
		SetDlgItemText(IDC_DATE, t.Format("%Y/%m/%d"));
		SetDlgItemText(IDC_TIME, t.Format("%H:%M:%S"));
		SetDlgItemText(IDC_MSG, msg->text.c_str());
	}
	delete msg;

	GetDlgItem(IDC_NEXT_MSG)->EnableWindow(icqLink->findPendingMsg(uin) != NULL);
}

void CViewMsgDlg::OnCancel() 
{
	CRect rc;
	GetWindowRect(rc);
	contact->winX = rc.left;
	contact->winY = rc.top;

	DestroyWindow();
}

void CViewMsgDlg::PostNcDestroy() 
{
	delete this;
}

void CViewMsgDlg::OnOK() 
{
	createSendMsgDlg();
	DestroyWindow();
}

void CViewMsgDlg::OnMenuQuickReply() 
{
#define IDM_QUICK_REPLY	1000
	
	CMenu menu;
	menu.CreatePopupMenu();

	IcqOption &options = icqLink->options;
	StrList l;
	IcqDB::loadQuickReply(l);
	StrList::iterator it;
	UINT id = IDM_QUICK_REPLY;

	CString str;
	for (it = l.begin(); it != l.end(); ++it) {
		CString str = (*it).c_str();
		if (str.GetLength() > 20)
			str = str.Left(20) + "...";
		menu.AppendMenu(
			str == options.quickReplyText.c_str() ? MF_CHECKED : MF_UNCHECKED,
			id++, str);
	}
	str.LoadString(IDS_QUICK_REPLY_SETTING);
	menu.AppendMenu(MF_STRING, id++, str);

	CRect rc;
	GetDlgItem(IDC_MENU_QUICK_REPLY)->GetWindowRect(rc);
	id = menu.TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, this);
	if (id == 0)
		return;

	id -= IDM_QUICK_REPLY;
	if (id == menu.GetMenuItemCount() - 1)
		((CIcqDlg *) AfxGetMainWnd())->showOptions(1);
	else {
		int i = 0;
		for (it = l.begin(); it != l.end() && i < id; ++it, ++i)
			;
		if (it != l.end()) {
			options.quickReplyText = *it;
			IcqDB::saveOptions(options);
			OnQuickReply();
		}
	}
}

void CViewMsgDlg::OnQuickReply() 
{
	string &str = icqLink->options.quickReplyText;
	if (!str.empty()) {
		CSendMsgDlg *win = createSendMsgDlg();
		win->sendMessage(str.c_str());
		DestroyWindow();
	}
}

void CViewMsgDlg::OnPic() 
{
	CViewDetailDlg *win = new CViewDetailDlg(uin);
	win->Create(this);
}

void CViewMsgDlg::OnHistory() 
{
	if (!expanded) {
		expand();
		m_lstHistory.loadHistory(uin);
	} else {
		shrink();
		m_lstHistory.ResetContent();
	}
}
