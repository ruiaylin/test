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

// GroupMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupMsgDlg.h"
#include "GroupInfoDlg.h"
#include "icqlink.h"
#include "udpsession.h"
#include "icqgroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupMsgDlg dialog


CGroupMsgDlg::CGroupMsgDlg(IcqGroup *g, CWnd* pParent /*=NULL*/)
	: CMyDlg(CGroupMsgDlg::IDD, pParent), GroupWindow(g->id)
{
	//{{AFX_DATA_INIT(CGroupMsgDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	group = g;

	m_hIcon = getApp()->getGroupIcon(g);

	Create(IDD_GROUP_MSG);
}

void CGroupMsgDlg::onAck()
{
	m_msgEdit.SetWindowText("");
	enableControls();

	m_msgView.appendMsg(msgSent, RGB(0, 128, 0));
	GetDlgItem(IDC_MSG_EDIT)->SetFocus();
}

void CGroupMsgDlg::onSendError()
{
	if (myMessageBox(IDS_SEND_RETRY, IDS_SORRY, this, MB_YESNO | MB_ICONERROR) == IDYES)
		seq = getUdpSession()->sendGroupMessage(id, msgSent.text.c_str());
	else
		enableControls();
}

void CGroupMsgDlg::sendMessage(IcqMsg &msg)
{
	TextOutStream out;
	msg.encode(out);
	seq = getUdpSession()->sendGroupMessage(id, out);
}

BOOL CGroupMsgDlg::CreateFormatBar()
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

void CGroupMsgDlg::enableControls(BOOL enable)
{
	m_msgEdit.SetReadOnly(!enable);
	GetDlgItem(IDC_SEND)->EnableWindow(enable);
}

void CGroupMsgDlg::fillMsg(IcqMsg &msg, LPCTSTR text)
{
	msg.type = MSG_TEXT;
	msg.qid.uin = 0;
	msg.qid.domain = "";
	msg.when = time(NULL);
	msg.flags = 0;
	msg.text = text;

	m_msgEdit.getMsgFormat(msg.format);
}

void CGroupMsgDlg::onRecvMessage(IcqMsg &msg)
{
	setTop();
	m_msgView.appendMsg(msg, RGB(0, 0, 255));
}

void CGroupMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupMsgDlg)
	DDX_Control(pDX, IDC_GROUP_DETAIL, m_groupButton);
	DDX_Control(pDX, IDC_MSG_VIEW, m_msgView);
	DDX_Control(pDX, IDC_MSG_EDIT, m_msgEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupMsgDlg, CMyDlg)
	//{{AFX_MSG_MAP(CGroupMsgDlg)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(IDC_GROUP_DETAIL, OnGroupDetail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupMsgDlg message handlers

BOOL CGroupMsgDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	CreateFormatBar();

	m_msgEdit.setToolBar(&m_wndFormatBar);
	m_msgEdit.SetEventMask(m_msgEdit.GetEventMask() | ENM_CHANGE | ENM_MOUSEEVENTS);
	m_msgEdit.LimitText(MAX_MSG_LEN);

	m_wndFormatBar.SetOwner(&m_msgEdit);
	m_wndFormatBar.SyncToView();

	m_groupButton.SetIcon(IDI_GROUP);

	CString str;
	str.Format("%s(%lu)", group->name.c_str(), id);
	SetWindowText(str);

	m_msgEdit.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGroupMsgDlg::PostNcDestroy() 
{
	delete this;
}

void CGroupMsgDlg::OnCancel() 
{
	DestroyWindow();
}

void CGroupMsgDlg::OnSend() 
{
	CString text;
	m_msgEdit.getText(text);

	if (text.IsEmpty()) {
		myMessageBox(IDS_MESSAGE_EMPTY, IDS_ERROR, this);
		return;
	}

	fillMsg(msgSent, text);
	enableControls(FALSE);

	sendMessage(msgSent);
}

void CGroupMsgDlg::OnGroupDetail() 
{
	new CGroupInfoDlg(group);
}

BOOL CGroupMsgDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		pMsg->hwnd == m_msgEdit &&
		(GetKeyState(VK_CONTROL) & 0x80))
	{
		OnSend();
		return TRUE;
	}
	
	return CMyDlg::PreTranslateMessage(pMsg);
}
