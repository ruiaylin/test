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
#include "udpsession.h"
#include "icqlink.h"
#include "icqdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_SENDMSG			1001
#define IDT_ANIMATE			1002

#define TCP_SEND_TIMEOUT	20

static CHARFORMAT lastFormat;


/////////////////////////////////////////////////////////////////////////////
// CSendMsgDlg dialog


CSendMsgDlg::CSendMsgDlg(IcqContact *c, CWnd* pParent /*=NULL*/)
: CMyDlg(CSendMsgDlg::IDD, pParent), IcqWindow(WIN_SEND_MESSAGE, c ? &c->qid : NULL)
{
	//{{AFX_DATA_INIT(CSendMsgDlg)
	m_ipport = _T("");
	//}}AFX_DATA_INIT

	contact = c;
	viewHistory = FALSE;
	isMultiple = FALSE;
	frame = 0;

	if (c) {
		m_hIcon = AfxGetApp()->LoadIcon(IDI_SENDMSG);

		m_qid = c->qid.toString();
		m_nick = c->nick.c_str();
		m_email = c->email.c_str();

		in_addr addr;
		addr.s_addr = htonl(c->ip);
		m_ipport.Format("%s:%d", inet_ntoa(addr), c->port);

	} else {
		m_hIcon = AfxGetApp()->LoadIcon(IDI_SYSMSG);
		m_qid.LoadString(IDS_TOALL);
	}

	Create(IDD, GetDesktopWindow());
}

BOOL CSendMsgDlg::isDialogMode()
{
	return (contact && contact->flags.test(CF_DIALOGMODE));
}

void CSendMsgDlg::fillMsg(IcqMsg &msg, LPCTSTR text)
{
	msg.type = MSG_TEXT;
	msg.qid = qid;
	msg.when = time(NULL);
	msg.flags = 0;
	msg.text = text;

	m_msgEdit.getMsgFormat(msg.format);
}

bool CSendMsgDlg::isSeq(uint32 seq)
{
	if (!isMultiple)
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
	m_msgEdit.SetReadOnly(!enable);
	GetDlgItem(IDC_SEND)->EnableWindow(enable);
	GetDlgItem(IDC_GROUPSEND)->EnableWindow(enable);
	if (!enable)
		SetTimer(IDT_ANIMATE, 300, NULL);
	else
		KillTimer(IDT_ANIMATE);
}

void CSendMsgDlg::resize()
{
	int w = wholeSize.cx;
	int h = wholeSize.cy;

	if (!viewHistory)
		h -= lowerHeight;
	if (!isDialogMode())
		h -= upperHeight;
	if (!isMultiple) {
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
	if (isDialogMode())
		str.LoadString(IDS_CHATMODE_NORMAL);
	else {
		str.LoadString(IDS_CHATMODE_DIALOG);
		offset = -offset;
	}
	SetDlgItemText(IDC_CHATMODE, str);
	m_msgEdit.SetFocus();

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
	m_recpts.ModifyStyle(TVS_CHECKBOXES, 0);
	m_recpts.ModifyStyle(0, TVS_CHECKBOXES);

	m_recpts.SetImageList(&getApp()->smallImageList, TVSIL_NORMAL);

	MyICQCtrl &outbar = ((CIcqDlg *) AfxGetMainWnd())->outbarCtrl;
	int nrFolders = outbar.getFolderCount();
	CString name;

	for (int i = 0; i < nrFolders - 1; ++i) {
		if (i == nrFolders - 3)
			continue;

		outbar.getFolderName(i, name);

		int image = getApp()->iconIndex(ICON_FOLDER);
		HTREEITEM parent = m_recpts.InsertItem(name, image, image);
		m_recpts.SetItemState(parent, 0, LVIS_STATEIMAGEMASK);

		int nrItems = outbar.getItemCount(i);
		for (int j = 0; j < nrItems; ++j) {
			IcqContact *c = outbar.contact(i, j);
			if (c->status == STATUS_OFFLINE)
				continue;

			name.Format("%s (%s)", c->nick.c_str(), c->qid.toString());

			int image = getApp()->getFaceIndex(c->face, c->status);
			HTREEITEM item = m_recpts.InsertItem(name, image, image, parent);
			m_recpts.SetItemData(item, (DWORD) new QID(c->qid));
			if (c->qid == qid)
				m_recpts.SetCheck(item);
		}
		m_recpts.Expand(parent, TVE_EXPAND);
	}
}

void CSendMsgDlg::onAck(uint32 seq)
{
	if (!isMultiple)
		IcqDB::saveMsg(msgSent);
	else {
		QID *qid = NULL;
		list<SEQ>::iterator it;
		for (it = seqList.begin(); it != seqList.end(); ++it) {
			if ((*it).seq == seq) {
				qid = (*it).qid;
				seqList.erase(it);
				break;
			}
		}
		if (qid) {
			msgSent.qid = *qid;
			IcqDB::saveMsg(msgSent);
		}
	}

	if (!isMultiple || seqList.empty()) {
		KillTimer(IDT_SENDMSG);
		m_msgEdit.SetWindowText("");
		enableControls();

		if (isDialogMode()) {
			m_msgView.appendMsg(msgSent, RGB(0, 128, 0));
			GetDlgItem(IDC_MSG_EDIT)->SetFocus();
		} else
			OnCancel();
	}
}

void CSendMsgDlg::onSendError(uint32 seq)
{
	if (!isMultiple) {
		if (myMessageBox(IDS_SEND_RETRY, IDS_SORRY, this, MB_YESNO | MB_ICONERROR) == IDYES)
			seq = getUdpSession()->sendMessage(MSG_TEXT, qid, msgSent.text.c_str());
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
	setTop();
	m_msgView.appendMsg(msg, RGB(0, 0, 255));
}

void CSendMsgDlg::sendMessage(const char *text)
{
	m_msgEdit.setText(text);
	OnSend();
}

void CSendMsgDlg::loadHistory()
{
	m_lstHistory.loadHistory(qid);
}

BOOL CSendMsgDlg::groupSend()
{
	seqList.clear();

	HTREEITEM item = m_recpts.GetFirstVisibleItem();
	while (item) {
		QID *qid = (QID *) m_recpts.GetItemData(item);
		if (qid && m_recpts.GetCheck(item)) {
			msgSent.qid = *qid;
			uint32 seq = icqLink->sendMessage(msgSent);
			SEQ node = { qid, seq };
			seqList.push_back(node);
		}
		item = m_recpts.GetNextVisibleItem(item);
	}

	if (seqList.empty()) {
		myMessageBox(IDS_UIN_EMPTY, IDS_ERROR, this, MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

void CSendMsgDlg::broadcastMsg()
{
	TextOutStream out;
	msgSent.encode(out);
	seq = getUdpSession()->sendBCMsg(MSG_TEXT, out);
}

BOOL CSendMsgDlg::CreateFormatBar()
{
	if (!m_wndFormatBar.create(this)) {
		TRACE0("Failed to create FormatBar\n");
		return FALSE;      // fail to create
	}

	CRect rc;
	GetDlgItem(IDC_TOOLBAR)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_wndFormatBar.MoveWindow(rc);

	return TRUE;
}

void CSendMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendMsgDlg)
	DDX_Control(pDX, IDC_MSG_EDIT, m_msgEdit);
	DDX_Control(pDX, IDC_RECIPIENTS, m_recpts);
	DDX_Control(pDX, IDC_MSG_HISTORY, m_lstHistory);
	DDX_Control(pDX, IDC_MSG_VIEW, m_msgView);
	DDX_Control(pDX, IDC_FACE, m_btnPic);
	DDX_Text(pDX, IDC_UIN, m_qid);
	DDX_Text(pDX, IDC_IPPORT, m_ipport);
	DDX_Text(pDX, IDC_NICK, m_nick);
	DDX_Text(pDX, IDC_EMAIL, m_email);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendMsgDlg, CMyDlg)
	//{{AFX_MSG_MAP(CSendMsgDlg)
	ON_BN_CLICKED(IDC_FACE, OnFace)
	ON_BN_CLICKED(IDC_HISTORY, OnHistory)
	ON_BN_CLICKED(IDC_CHATMODE, OnChatMode)
	ON_BN_CLICKED(IDC_ENTER, OnEnter)
	ON_BN_CLICKED(IDC_CTRLENTER, OnCtrlEnter)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_LBN_DBLCLK(IDC_MSG_HISTORY, OnDblclkHistory)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GROUPSEND, OnGroupSend)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendMsgDlg message handlers

BOOL CSendMsgDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	CreateFormatBar();

	m_msgEdit.setToolBar(&m_wndFormatBar);
	m_msgEdit.SetEventMask(m_msgEdit.GetEventMask() | ENM_CHANGE | ENM_MOUSEEVENTS);
	m_msgEdit.SetDefaultCharFormat(lastFormat);
	m_msgEdit.LimitText(MAX_MSG_LEN);

	m_wndFormatBar.SetOwner(&m_msgEdit);
	m_wndFormatBar.SyncToView();

	defFormat.cbSize = sizeof(defFormat);
	defFormat.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_FACE | CFM_SIZE | CFM_COLOR;
	m_msgView.GetDefaultCharFormat(defFormat);
	m_msgView.AutoURLDetect(TRUE);

	initTree();

	int face = (contact ? contact->face : 0);
	m_btnPic.SetIcon(getApp()->getLargeFace(face));

	CheckRadioButton(IDC_ENTER, IDC_CTRLENTER,
		(contact && contact->flags.test(CF_SENDENTER) ? IDC_ENTER : IDC_CTRLENTER));

	if (!contact) {
		CString str;
		str.LoadString(IDS_SYSMSG);
		SetWindowText(str);

		GetDlgItem(IDC_GROUPSEND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HISTORY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHATMODE)->ShowWindow(SW_HIDE);
	}

	CRect rc;
	GetWindowRect(rc);
	wholeSize = rc.Size();
	GetDlgItem(IDC_UPPER)->GetWindowRect(rc);
	upperHeight = rc.Height();
	GetDlgItem(IDC_LOWER)->GetWindowRect(rc);
	lowerHeight = rc.Height();

	setChatMode(!isDialogMode());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendMsgDlg::PostNcDestroy() 
{
	delete this;
}

void CSendMsgDlg::OnCancel() 
{
	lastFormat.cbSize = sizeof(CHARFORMAT);
	lastFormat.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE |
		CFM_COLOR | CFM_FACE | CFM_SIZE;
	m_msgEdit.GetDefaultCharFormat(lastFormat);
	
	DestroyWindow();
}

void CSendMsgDlg::OnSend() 
{
	CString text;
	m_msgEdit.getText(text);

	if (text.IsEmpty()) {
		myMessageBox(IDS_MESSAGE_EMPTY, IDS_ERROR, this);
		return;
	}

	fillMsg(msgSent, text);

	if (!contact)
		broadcastMsg();
	else if (isMultiple) {
		if (!groupSend())
			return;
	} else {
		seq = icqLink->sendMessage(msgSent);
		if (seq > 0xffff)
			SetTimer(IDT_SENDMSG, TCP_SEND_TIMEOUT * 1000, NULL);
		else
			msgSent.flags |= MF_RELAY;
	}

	enableControls(FALSE);
	if (!isDialogMode())
		ShowWindow(SW_SHOWMINIMIZED);
}

void CSendMsgDlg::OnFace() 
{
	if (qid.uin)
		new CViewDetailDlg(qid, this);
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
		PtrList l;
		IcqDB::loadMsg(&qid, l, 3);
		for (int i = 0; !l.empty(); ++i) {
			IcqMsg *msg = (IcqMsg *) l.front();
			l.pop_front();
			m_msgView.appendMsg(*msg, RGB(128, 128, 128));
			delete msg;
		}
	} else
		m_msgView.SetWindowText("");
}

void CSendMsgDlg::OnEnter() 
{
	if (contact)
		contact->flags.set(CF_SENDENTER);
}

void CSendMsgDlg::OnCtrlEnter() 
{
	if (contact)
		contact->flags.set(CF_SENDENTER, false);
}

BOOL CSendMsgDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		pMsg->hwnd == m_msgEdit &&
		(IsDlgButtonChecked(IDC_ENTER) || (GetKeyState(VK_CONTROL) & 0x80)))
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
		m_msgEdit.insertText(str);
		m_msgEdit.ReplaceSel("\r\n");
	}
}

void CSendMsgDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_SENDMSG) {
		KillTimer(nIDEvent);
		msgSent.flags |= MF_RELAY;
		icqLink->cancelSendMessage(qid);
		seq = icqLink->sendMessage(msgSent);

	} else if (nIDEvent == IDT_ANIMATE) {
		frame ^= 1;
		int status = (frame ? STATUS_AWAY : STATUS_ONLINE);
		if (contact)
			m_btnPic.SetIcon(getApp()->getLargeFace(contact->face, status));

	} else
		CMyDlg::OnTimer(nIDEvent);
}

void CSendMsgDlg::OnGroupSend() 
{
	isMultiple = !isMultiple;

	CString str;
	if (isMultiple) {
		str.LoadString(IDS_SINGLE_RECPTS);
		viewHistory = FALSE;
		if (contact->flags.test(CF_DIALOGMODE))
			OnChatMode();
	} else
		str.LoadString(IDS_MULTIPLE_RECPTS);
	SetDlgItemText(IDC_GROUPSEND, str);

	BOOL b = !isMultiple;
	GetDlgItem(IDC_HISTORY)->EnableWindow(b);
	GetDlgItem(IDC_CHATMODE)->EnableWindow(b);

	resize();
}

void CSendMsgDlg::OnDestroy() 
{
	HTREEITEM item = m_recpts.GetFirstVisibleItem();

	while (item) {
		QID *qid = (QID *) m_recpts.GetItemData(item);
		if (qid)
			delete qid;

		item = m_recpts.GetNextVisibleItem(item);
	}
	
	CMyDlg::OnDestroy();
}
