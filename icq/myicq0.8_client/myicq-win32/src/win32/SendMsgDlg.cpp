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

// SendMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SendMsgDlg.h"
#include "myicqDlg.h"
#include "ViewDetailDlg.h"
#include "serversession.h"
#include "icqlink.h"
#include "icqdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_SENDMSG		1001
#define IDT_ANIMATE		1002

/////////////////////////////////////////////////////////////////////////////
// CSendMsgDlg dialog


CSendMsgDlg::CSendMsgDlg(IcqContact *c, CWnd* pParent /*=NULL*/)
:CMyDlg(CSendMsgDlg::IDD, pParent), IcqWindow(WIN_SEND_MESSAGE, c->uin)
{
	//{{AFX_DATA_INIT(CSendMsgDlg)
	m_msgText = _T("");
	m_ipport = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SENDMSG);
	contact = c;
	viewHistory = FALSE;
	groupSend = FALSE;
	frame = 0;

	m_uin.Format("%lu", c->uin);
	m_nick = c->nick.c_str();
	m_email = c->email.c_str();

	in_addr addr;
	addr.s_addr = htonl(c->ip);
	m_ipport.Format("%s:%d", inet_ntoa(addr), c->port);
}

bool CSendMsgDlg::isSeq(uint32 seq)
{
	if (!groupSend)
		return IcqWindow::isSeq(seq);

	list<SEQ>::iterator it;
	for (it = seqList.begin(); it != seqList.end(); ++it) {
		if ((*it).seq == seq)
			return true;
	}
	return false;
}

void CSendMsgDlg::enableControls(BOOL enable)
{
	m_edtMsg.EnableWindow(enable);
	GetDlgItem(IDC_SEND)->EnableWindow(enable);
	GetDlgItem(IDC_GROUPSEND)->EnableWindow(enable);
	if (!enable)
		SetTimer(IDT_ANIMATE, 300, NULL);
	else {
		KillTimer(IDT_ANIMATE);
		m_btnPic.SetIcon(getApp()->largeImageList.ExtractIcon(contact->face));
	}
}

void CSendMsgDlg::resize()
{
	int w = wholeSize.cx;
	int h = wholeSize.cy;

	if (!viewHistory)
		h -= lowerHeight;
	if (!contact->flags.test(CF_DIALOGMODE))
		h -= upperHeight;
	if (!groupSend) {
		CRect rc;
		GetDlgItem(IDC_RECIPIENTS_STATIC)->GetWindowRect(rc);
		w -= rc.Width() + 2;
	}
	SetWindowPos(NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
}

void CSendMsgDlg::setChatMode(BOOL moveControls)
{
	int offset = upperHeight;
	CString str;
	if (contact->flags.test(CF_DIALOGMODE))
		str.LoadString(IDS_CHATMODE_NORMAL);
	else {
		str.LoadString(IDS_CHATMODE_DIALOG);
		offset = -offset;
	}
	SetDlgItemText(IDC_CHATMODE, str);
	m_edtMsg.SetFocus();

	if (moveControls) {
		for (CWnd *p = GetWindow(GW_CHILD); p; p = p->GetNextWindow()) {
			CRect rc;
			p->GetWindowRect(rc);
			ScreenToClient(rc);
			rc.OffsetRect(0, offset);
			p->MoveWindow(rc, FALSE);
		}
	}
	resize();
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}

void CSendMsgDlg::initTree()
{
	m_recipients.ModifyStyle(TVS_CHECKBOXES, 0);
	m_recipients.ModifyStyle(0, TVS_CHECKBOXES);

	m_recipients.SetImageList(&getApp()->smallImageList, TVSIL_NORMAL);

	MyICQCtrl &outbar = ((CIcqDlg *) AfxGetMainWnd())->outbarCtrl;
	int nrFolders = outbar.getFolderCount();
	CString name;

	for (int i = 0; i < nrFolders - 1; ++i) {
		outbar.getFolderName(i, name);

		int image = getApp()->iconIndex(ICON_FOLDER);
		HTREEITEM parent = m_recipients.InsertItem(name, image, image);
		m_recipients.SetItemState(parent, 0, LVIS_STATEIMAGEMASK);

		int nrItems = outbar.getItemCount(i);
		for (int j = 0; j < nrItems; ++j) {
			IcqContact *c = outbar.contact(i, j);
			if (c->status == STATUS_ONLINE) {
				name.Format("%s (%lu)", c->nick.c_str(), c->uin);

				HTREEITEM item = m_recipients.InsertItem(name, getApp()->getImageIndex(c->face),
					getApp()->getImageIndex(c->face, STATUS_AWAY), parent);
				m_recipients.SetItemData(item, c->uin);
				if (c->uin == uin)
					m_recipients.SetCheck(item);
			}
		}
		m_recipients.Expand(parent, TVE_EXPAND);
	}
}

void CSendMsgDlg::appendMsg(IcqMsg &msg, COLORREF color)
{
	CString str;
	CTime t(msg.when);
	CString strTime = t.Format("%H:%M:%S");
	str.Format("%s (%s): %s\r\n",
		(msg.flags & MF_RECEIVED) ? contact->nick.c_str() : icqLink->myInfo.nick.c_str(),
		(LPCTSTR) strTime, msg.text.c_str());
	if (msg.flags & MF_RELAY) {
		CString tmp;
		tmp.LoadString(IDS_SERVER_RELAY);
		str += tmp + "\r\n";
	}

	long start, end;
	m_ctlMsgView.SetSel(-1, -1);
	m_ctlMsgView.GetSel(start, end);
	m_ctlMsgView.ReplaceSel(str);
	m_ctlMsgView.SetSel(start, -1);

	CHARFORMAT cf;
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0;
	cf.crTextColor = color;
	m_ctlMsgView.SetSelectionCharFormat(cf);
	m_ctlMsgView.SetSel(-1, -1);
}

void CSendMsgDlg::saveWindowPos()
{
	if (!IsIconic()) {
		CRect rc;
		GetWindowRect(rc);
		contact->winX = rc.left;
		contact->winY = rc.top;
	}
}

void CSendMsgDlg::onAck(uint32 seq)
{
	if (!groupSend) {
		if (seq > 0xffff)
			KillTimer(IDT_SENDMSG);
	
		enableControls();
		IcqDB::saveMsg(msgSent);

	} else {
		uint32 to = 0;
		list<SEQ>::iterator it;
		for (it = seqList.begin(); it != seqList.end(); ++it) {
			if ((*it).seq == seq) {
				to = (*it).uin;
				seqList.erase(it);
				break;
			}
		}
		msgSent.uin = to;
		IcqDB::saveMsg(msgSent);

		if (seqList.empty()) {
			KillTimer(IDT_SENDMSG);
			enableControls();
		}
	}

	appendMsg(msgSent, RGB(0, 128, 0));
	m_msgText.Empty();
	UpdateData(FALSE);

	if (contact->flags.test(CF_DIALOGMODE))
		GetDlgItem(IDC_MSG_EDIT)->SetFocus();
	else
		OnCancel();
}

void CSendMsgDlg::onSendError(uint32 seq)
{
	if (!groupSend) {
		if (myMessageBox(IDS_SEND_RETRY, IDS_SORRY, this, MB_YESNO | MB_ICONERROR) == IDYES)
			seq = serverSession()->sendMessage(MSG_TEXT, uin, m_msgText);
		else
			enableControls();

	} else {
		list<SEQ>::iterator it;
		for (it = seqList.begin(); it != seqList.end(); ++it) {
			if ((*it).seq == seq) {
				seqList.erase(it);
				break;
			}
		}
		if (seqList.empty())
			myMessageBox(IDS_GROUPSEND_FAILED, IDS_SORRY, this, MB_ICONERROR);
	}
}

void CSendMsgDlg::onRecvMessage(IcqMsg &msg)
{
	appendMsg(msg, RGB(0, 0, 255));

	ShowWindow(SW_NORMAL);
	BringWindowToTop();
}

void CSendMsgDlg::sendMessage(const char *text)
{
	m_msgText = text;
	UpdateData(FALSE);
	OnSend();
}

void CSendMsgDlg::loadHistory()
{
	m_lstHistory.loadHistory(uin);
}

BOOL CSendMsgDlg::onGroupSend()
{
	seqList.clear();

	HTREEITEM item = m_recipients.GetFirstVisibleItem();
	while (item) {
		uint32 to = m_recipients.GetItemData(item);
		if (to && m_recipients.GetCheck(item)) {
			uint32 seq = icqLink->sendMessage(MSG_TEXT, to, m_msgText);
			SEQ node;
			node.uin = to;
			node.seq = seq;
			seqList.push_back(node);
		}
		item = m_recipients.GetNextVisibleItem(item);
	}

	if (seqList.empty()) {
		myMessageBox(IDS_UIN_EMPTY, IDS_ERROR, this, MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

void CSendMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendMsgDlg)
	DDX_Control(pDX, IDC_RECIPIENTS, m_recipients);
	DDX_Control(pDX, IDC_MSG_EDIT, m_edtMsg);
	DDX_Control(pDX, IDC_MSG_HISTORY, m_lstHistory);
	DDX_Control(pDX, IDC_MSG_VIEW, m_ctlMsgView);
	DDX_Control(pDX, IDC_PIC, m_btnPic);
	DDX_Text(pDX, IDC_MSG_EDIT, m_msgText);
	DDV_MaxChars(pDX, m_msgText, 512);
	DDX_Text(pDX, IDC_UIN, m_uin);
	DDX_Text(pDX, IDC_IPPORT, m_ipport);
	DDX_Text(pDX, IDC_NICK, m_nick);
	DDX_Text(pDX, IDC_EMAIL, m_email);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendMsgDlg, CMyDlg)
	//{{AFX_MSG_MAP(CSendMsgDlg)
	ON_BN_CLICKED(IDC_PIC, OnPic)
	ON_BN_CLICKED(IDC_HISTORY, OnHistory)
	ON_BN_CLICKED(IDC_CHATMODE, OnChatMode)
	ON_BN_CLICKED(IDC_ENTER, OnEnter)
	ON_BN_CLICKED(IDC_CTRLENTER, OnCtrlEnter)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_LBN_DBLCLK(IDC_MSG_HISTORY, OnDblclkHistory)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GROUPSEND, OnGroupSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendMsgDlg message handlers

BOOL CSendMsgDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	initTree();

	int x = contact->winX;
	int y = contact->winY;
	if (x != -1 || y != -1)
		SetWindowPos(NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	
	m_btnPic.SetIcon(getApp()->largeImageList.ExtractIcon(contact->face));
	in_addr addr;
	addr.s_addr = htonl(contact->ip);
	SetDlgItemText(IDC_ADDR, inet_ntoa(addr));
	SetDlgItemInt(IDC_PORT, contact->port, FALSE);

	CheckRadioButton(IDC_ENTER, IDC_CTRLENTER,
		(contact->flags.test(CF_SENDENTER) ? IDC_ENTER : IDC_CTRLENTER));

	CRect rc;
	GetWindowRect(rc);
	wholeSize = rc.Size();
	GetDlgItem(IDC_UPPER)->GetWindowRect(rc);
	upperHeight = rc.Height();
	GetDlgItem(IDC_LOWER)->GetWindowRect(rc);
	lowerHeight = rc.Height();

	setChatMode(!contact->flags.test(CF_DIALOGMODE));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendMsgDlg::PostNcDestroy() 
{
	delete this;
}

void CSendMsgDlg::OnCancel() 
{
	saveWindowPos();

	DestroyWindow();
}

void CSendMsgDlg::OnSend() 
{
	UpdateData();
	if (m_msgText.IsEmpty()) {
		myMessageBox(IDS_MESSAGE_EMPTY, IDS_ERROR, this);
		return;
	}

	msgSent.flags = 0;
	msgSent.when = time(NULL);
	msgSent.type = MSG_TEXT;
	msgSent.text = m_msgText;

	if (groupSend) {
		if (!onGroupSend())
			return;
	} else {
		msgSent.uin = uin;
		seq = icqLink->sendMessage(MSG_TEXT, uin, m_msgText);
		if (seq > 0xffff)
			SetTimer(IDT_SENDMSG, 30000, NULL);
		else
			msgSent.flags |= MF_RELAY;
	}

	enableControls(FALSE);
	if (!contact->flags.test(CF_DIALOGMODE)) {
		saveWindowPos();
		ShowWindow(SW_SHOWMINIMIZED);
	}
}

void CSendMsgDlg::OnPic() 
{
	CViewDetailDlg *win = new CViewDetailDlg(uin);
	win->Create(this);
}

void CSendMsgDlg::OnHistory() 
{
	viewHistory = !viewHistory;
	resize();

	if (viewHistory)
		loadHistory();
	else
		m_lstHistory.ResetContent();
}

void CSendMsgDlg::OnChatMode() 
{
	contact->flags.flip(CF_DIALOGMODE);
	IcqDB::saveContact(*contact);

	setChatMode();

	if (contact->flags.test(CF_DIALOGMODE)) {
		// FIXME: only need to load the last 3 messages
		PtrList l;
		IcqDB::loadMsg(uin, l);
		for (int i = 0; !l.empty(); ++i) {
			IcqMsg *msg = (IcqMsg *) l.back();
			l.pop_back();
			if (i < 3)
				appendMsg(*msg, RGB(128, 128, 128));
			delete msg;
		}
	} else
		m_ctlMsgView.SetWindowText("");
}

void CSendMsgDlg::OnEnter() 
{
	contact->flags.set(CF_SENDENTER);
}

void CSendMsgDlg::OnCtrlEnter() 
{
	contact->flags.set(CF_SENDENTER, false);
}

BOOL CSendMsgDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		pMsg->hwnd == m_edtMsg &&
		(contact->flags.test(CF_SENDENTER) || (GetKeyState(VK_CONTROL) & 0x80)))
	{
		OnSend();
		return TRUE;
	}
	return CMyDlg::PreTranslateMessage(pMsg);
}

void CSendMsgDlg::OnDblclkHistory() 
{
	int index = m_lstHistory.GetCurSel();
	if (index >= 0) {
		CString str;
		m_lstHistory.GetText(index, str);
		str += "\r\n";
		m_edtMsg.ReplaceSel(str, TRUE);
	}
}

void CSendMsgDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_SENDMSG) {
		KillTimer(nIDEvent);
		msgSent.flags |= MF_RELAY;
		seq = serverSession()->sendMessage(MSG_TEXT, uin, m_msgText);

	} else if (nIDEvent == IDT_ANIMATE) {
		frame ^= 1;
		int i = contact->face;
		if (frame == 1)
			i += getApp()->nrFaces * 2;
		m_btnPic.SetIcon(getApp()->largeImageList.ExtractIcon(i));

	} else
		CMyDlg::OnTimer(nIDEvent);
}

void CSendMsgDlg::OnGroupSend() 
{
	groupSend = !groupSend;

	CString str;
	if (groupSend) {
		str.LoadString(IDS_SINGLE_RECIPIENTS);
		viewHistory = FALSE;
		if (contact->flags.test(CF_DIALOGMODE))
			OnChatMode();
	} else
		str.LoadString(IDS_MULTIPLE_RECIPIENTS);
	SetDlgItemText(IDC_GROUPSEND, str);

	BOOL b = !groupSend;
	GetDlgItem(IDC_HISTORY)->EnableWindow(b);
	GetDlgItem(IDC_CHATMODE)->EnableWindow(b);

	resize();
}
