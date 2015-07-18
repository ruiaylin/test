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

// SysMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SysMsgDlg.h"
#include "ViewDetailDlg.h"
#include "udpsession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_ANIMATE		1001

enum {
	ACTION_NONE,
	ACTION_ACCEPT_REJECT,
	ACTION_SEND_REQUEST
};

/////////////////////////////////////////////////////////////////////////////
// CSysMsgDlg dialog

CSysMsgDlg::CSysMsgDlg(IcqMsg *msg, BOOL autoDelete, CWnd* pParent /*=NULL*/)
	: CMyDlg(CSysMsgDlg::IDD, pParent), IcqWindow(WIN_SYS_MESSAGE, &msg->qid)
{
	//{{AFX_DATA_INIT(CSysMsgDlg)
	//}}AFX_DATA_INIT
	this->msg = msg;
	this->autoDelete = autoDelete;

	init();
}

CSysMsgDlg::CSysMsgDlg(QID &qid, CWnd* pParent /*=NULL*/)
	: CMyDlg(CSysMsgDlg::IDD, pParent), IcqWindow(WIN_ADD_FRIEND, &qid)
{
	msg = NULL;

	init();
}

CSysMsgDlg::~CSysMsgDlg()
{
	if (autoDelete && msg)
		delete msg;
}

void CSysMsgDlg::init()
{
	expanded = TRUE;
	lastAction = ACTION_NONE;

	Create(IDD, GetDesktopWindow());
}

void CSysMsgDlg::enableControls(BOOL enable)
{
	GetDlgItem(IDC_REQ_ACCEPT)->EnableWindow(enable);
	GetDlgItem(IDC_REQ_REJECT)->EnableWindow(enable);
	GetDlgItem(IDC_REQUEST)->EnableWindow(enable);

	GetDlgItem(IDC_ADD_FRIEND)->EnableWindow(
		enable && (icqLink->findContact(qid, CONTACT_FRIEND) != NULL));
}

void CSysMsgDlg::startAnim(BOOL start)
{
	if (start)
		SetTimer(IDT_ANIMATE, 400, NULL);
	else
		KillTimer(IDT_ANIMATE);
}

void CSysMsgDlg::shrink()
{
	int h = wholeSize.cy - lowerHeight;
	SetWindowPos(NULL, 0, 0, wholeSize.cx, h, SWP_NOZORDER | SWP_NOMOVE);
	expanded = FALSE;
}

void CSysMsgDlg::expand()
{
	SetWindowPos(NULL, 0, 0, wholeSize.cx, wholeSize.cy, SWP_NOZORDER | SWP_NOMOVE);
	expanded = TRUE;
}

void CSysMsgDlg::onAck(uint32 seq)
{
	CString str;
	str.LoadString(IDS_MSG_SENT);
	SetDlgItemText(IDC_MSG, str);

	if (lastAction == ACTION_ACCEPT_REJECT) {
		GetDlgItem(IDC_REQ_ACCEPT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REQ_REJECT)->ShowWindow(SW_HIDE);

	} else if (lastAction == ACTION_SEND_REQUEST)
		shrink();

	enableControls(TRUE);
	startAnim(FALSE);
}

void CSysMsgDlg::onSendError(uint32 seq)
{
	CString str;
	str.LoadString(IDS_TIMEOUT);
	SetDlgItemText(IDC_MSG, str);
	enableControls(FALSE);
	startAnim(FALSE);
	lastAction = ACTION_NONE;

	if (expanded)
		shrink();
}

void CSysMsgDlg::onAddFriendReply(uint8 result)
{
	enableControls(TRUE);
	startAnim(FALSE);

	CString str;

	if (result == ADD_FRIEND_ACCEPTED)
		str.LoadString(IDS_ADD_FRIEND_ACCEPTED);
	else if (result == ADD_FRIEND_REJECTED)
		str.LoadString(IDS_ADD_REJECTED);
	else if (result == ADD_FRIEND_AUTH_REQ) {
		str.LoadString(IDS_AUTH_REQUIRED);
		expand();
		GetDlgItem(IDC_REQUEST)->SetFocus();
	}

	SetDlgItemText(IDC_MSG, str);
}


void CSysMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysMsgDlg)
	DDX_Control(pDX, IDC_FACE, m_faceButton);
	DDX_Control(pDX, IDC_MSG, m_msgEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSysMsgDlg, CMyDlg)
	//{{AFX_MSG_MAP(CSysMsgDlg)
	ON_BN_CLICKED(IDC_REQ_ACCEPT, OnReqAccept)
	ON_BN_CLICKED(IDC_REQ_REJECT, OnReqReject)
	ON_BN_CLICKED(IDC_SEND_REQUEST, OnSendRequest)
	ON_BN_CLICKED(IDC_ADD_FRIEND, OnAddFriend)
	ON_BN_CLICKED(IDC_FACE, OnFace)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysMsgDlg message handlers

BOOL CSysMsgDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	CRect rc;
	GetWindowRect(rc);
	wholeSize = rc.Size();
	GetDlgItem(IDC_LOWER)->GetWindowRect(rc);
	lowerHeight = rc.Height();
	shrink();

	m_msgEdit.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));
	m_msgEdit.AutoURLDetect(TRUE);

	IcqContact *contact = icqLink->findContact(qid);

	if (msg) {
		uint8 face = 0;

		if (msg->type == MSG_AUTH_REQUEST) {
			TextInStream in(msg->text.c_str());
			string nick;
			in >> face >> nick;
			SetDlgItemText(IDC_NICK, nick.c_str());

		} else {
			GetDlgItem(IDC_REQ_ACCEPT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_REQ_REJECT)->ShowWindow(SW_HIDE);
			if (msg->type != MSG_ADDED)
				GetDlgItem(IDC_ADD_FRIEND)->ShowWindow(SW_HIDE);
			SetDlgItemText(IDC_NICK, qid.toString());
		}

		CString str;
		if (msg->type == MSG_TEXT)
			m_msgEdit.insertMsg(*msg);
		else {
			getMsgText(msg, str);
			m_msgEdit.SetWindowText(str);
		}

		m_faceButton.SetIcon(getApp()->getLargeFace(face));

		if (contact && contact->type == CONTACT_FRIEND)
			GetDlgItem(IDC_ADD_FRIEND)->EnableWindow(FALSE);

	} else {
		GetDlgItem(IDC_REQ_ACCEPT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REQ_REJECT)->ShowWindow(SW_HIDE);
		if (contact) {
			SetDlgItemText(IDC_NICK, contact->nick.c_str());
			m_faceButton.SetIcon(getApp()->getLargeFace(contact->face));
		}

		OnAddFriend();
	}

	SetDlgItemText(IDC_UIN, qid.toString());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSysMsgDlg::OnCancel() 
{
	DestroyWindow();
}

void CSysMsgDlg::PostNcDestroy() 
{
	delete this;
}

void CSysMsgDlg::OnFace() 
{
	new CViewDetailDlg(qid, this);
}

void CSysMsgDlg::OnReqAccept() 
{
	enableControls(FALSE);
	startAnim();
	lastAction = ACTION_ACCEPT_REJECT;

	seq = getUdpSession()->sendMessage(MSG_AUTH_ACCEPTED, qid, "");
}

void CSysMsgDlg::OnReqReject() 
{
	CString str;
	if (m_msgEdit.GetStyle() & ES_READONLY) {
		m_msgEdit.SetReadOnly(FALSE);
		str.LoadString(IDS_PROMPT_REJECT_REASON);
		m_msgEdit.SetWindowText(str);
		m_msgEdit.SetSel(0, -1);
		m_msgEdit.SetFocus();
	} else {
		enableControls(FALSE);
		startAnim();
		lastAction = ACTION_ACCEPT_REJECT;

		m_msgEdit.GetWindowText(str);
		seq = getUdpSession()->sendMessage(MSG_AUTH_REJECTED, qid, str);
	}
}

void CSysMsgDlg::OnSendRequest() 
{
	CString str;
	GetDlgItemText(IDC_REQUEST, str);
	enableControls(FALSE);
	startAnim();
	lastAction = ACTION_SEND_REQUEST;

	IcqUser &info = icqLink->myInfo;
	TextOutStream out;
	out << info.face << info.nick.c_str() << str;
	seq = getUdpSession()->sendMessage(MSG_AUTH_REQUEST, qid, out);
}

void CSysMsgDlg::OnAddFriend() 
{
	CString str;
	str.LoadString(IDS_ADD_FRIEND_REQ);
	SetDlgItemText(IDC_MSG, str);
	enableControls(FALSE);
	startAnim();

	seq = getUdpSession()->addFriend(qid);
}

void CSysMsgDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_ANIMATE) {
		static int frame;

		m_faceButton.SetIcon(getApp()->getLargeFace(frame));
		if (++frame >= NR_FACES)
			frame = 0;
	} else
		CMyDlg::OnTimer(nIDEvent);
}
