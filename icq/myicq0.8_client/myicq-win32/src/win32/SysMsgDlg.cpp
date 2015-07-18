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
#include "serversession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysMsgDlg dialog

CSysMsgDlg::CSysMsgDlg(IcqMsg *msg, BOOL autoDelete, CWnd* pParent /*=NULL*/)
	: CMyDlg(CSysMsgDlg::IDD, pParent), IcqWindow(WIN_SYS_MESSAGE, msg->uin)
{
	//{{AFX_DATA_INIT(CSysMsgDlg)
	//}}AFX_DATA_INIT
	this->msg = msg;
	this->autoDelete = autoDelete;
	expanded = TRUE;
}

CSysMsgDlg::CSysMsgDlg(uint32 uin, CWnd* pParent /*=NULL*/)
	: CMyDlg(CSysMsgDlg::IDD, pParent), IcqWindow(WIN_ADD_FRIEND, uin)
{
	msg = NULL;
	expanded = TRUE;
}

CSysMsgDlg::~CSysMsgDlg()
{
	if (autoDelete && msg)
		delete msg;
}

void CSysMsgDlg::enableControls(BOOL enable)
{
	GetDlgItem(IDC_REQ_ACCEPT)->EnableWindow(enable);
	GetDlgItem(IDC_REQ_REJECT)->EnableWindow(enable);
	GetDlgItem(IDC_REQUEST)->EnableWindow(enable);
	
	if (icqLink->findContact(uin))
		GetDlgItem(IDC_ADD_FRIEND)->EnableWindow(
			enable && (icqLink->findContact(uin) != NULL));
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
	DestroyWindow();
}

void CSysMsgDlg::onSendError(uint32 seq)
{
	CString str;
	str.LoadString(IDS_TIMEOUT);
	SetDlgItemText(IDC_MSG, str);
	m_btnPic.stop();
	enableControls(FALSE);
	if (expanded)
		shrink();
}

void CSysMsgDlg::onAddFriendReply(uint8 result)
{
	m_btnPic.stop();
	enableControls(TRUE);
	
	if (result == ADD_FRIEND_ACCEPTED)
		DestroyWindow();
	else if (result == ADD_FRIEND_AUTH_REQ) {
		CString str;
		str.LoadString(IDS_AUTH_REQUIRED);
		SetDlgItemText(IDC_MSG, str);

		expand();
		GetDlgItem(IDC_REQUEST)->SetFocus();

	} else if (result == ADD_FRIEND_REJECTED) {
		CString str;
		str.LoadString(IDS_ADD_REJECTED);
		SetDlgItemText(IDC_MSG, str);
	}
}


void CSysMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysMsgDlg)
	DDX_Control(pDX, IDC_MSG, m_edtMsg);
	DDX_Control(pDX, IDC_PIC, m_btnPic);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSysMsgDlg, CMyDlg)
	//{{AFX_MSG_MAP(CSysMsgDlg)
	ON_BN_CLICKED(IDC_PIC, OnPic)
	ON_BN_CLICKED(IDC_REQ_ACCEPT, OnReqAccept)
	ON_BN_CLICKED(IDC_REQ_REJECT, OnReqReject)
	ON_BN_CLICKED(IDC_SEND_REQUEST, OnSendRequest)
	ON_BN_CLICKED(IDC_ADD_FRIEND, OnAddFriend)
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

	m_btnPic.setImageList(&getApp()->largeImageList, getApp()->nrFaces);

	IcqContact *contact = icqLink->findContact(uin);

	if (msg) {
		uint8 pic = 0;
		string nick;

		if (msg->type == MSG_AUTH_REQUEST) {
			TextInStream in(msg->text.c_str());
			in >> pic >> nick;
		} else {
			GetDlgItem(IDC_REQ_ACCEPT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_REQ_REJECT)->ShowWindow(SW_HIDE);
			if (msg->type != MSG_ADDED)
				GetDlgItem(IDC_ADD_FRIEND)->ShowWindow(SW_HIDE);
		}
		SetDlgItemText(IDC_NICK, nick.c_str());
		CString str;
		getMsgText(msg, str);
		SetDlgItemText(IDC_MSG, str);
		m_btnPic.stop(pic);

		if (contact)
			GetDlgItem(IDC_ADD_FRIEND)->EnableWindow(FALSE);

	} else {
		GetDlgItem(IDC_REQ_ACCEPT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REQ_REJECT)->ShowWindow(SW_HIDE);
		if (contact) {
			SetDlgItemText(IDC_NICK, contact->nick.c_str());
			m_btnPic.stop(contact->face);
		}

		OnAddFriend();
	}

	SetDlgItemInt(IDC_UIN, uin, FALSE);

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


void CSysMsgDlg::OnPic() 
{
	CViewDetailDlg *win = new CViewDetailDlg(uin);
	win->Create(this);
}

void CSysMsgDlg::OnReqAccept() 
{
	enableControls(FALSE);
	seq = serverSession()->sendMessage(MSG_AUTH_ACCEPTED, uin, "");
}

void CSysMsgDlg::OnReqReject() 
{
	CString str;
	if (m_edtMsg.GetStyle() & ES_READONLY) {
		m_edtMsg.SetReadOnly(FALSE);
		str.LoadString(IDS_PROMPT_REJECT_REASON);
		m_edtMsg.SetWindowText(str);
		m_edtMsg.SetSel(0, -1);
		m_edtMsg.SetFocus();
	} else {
		enableControls(FALSE);

		m_edtMsg.GetWindowText(str);
		seq = serverSession()->sendMessage(MSG_AUTH_REJECTED, uin, str);
	}
}

void CSysMsgDlg::OnSendRequest() 
{
	CString str;
	GetDlgItemText(IDC_REQUEST, str);
	enableControls(FALSE);

	IcqUser &info = icqLink->myInfo;
	TextOutStream out;
	out << info.face << info.nick.c_str() << str;
	seq = serverSession()->sendMessage(MSG_AUTH_REQUEST, uin, out);
}

void CSysMsgDlg::OnAddFriend() 
{
	CString str;
	str.LoadString(IDS_ADD_FRIEND_REQ);
	SetDlgItemText(IDC_MSG, str);
	enableControls(FALSE);
	m_btnPic.start();

	seq = serverSession()->addFriend(uin);
}
