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

// SendRequestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SendRequestDlg.h"
#include "icqlink.h"
#include "tcpsession.h"
#include "icqplugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_ANIM	1001

/////////////////////////////////////////////////////////////////////////////
// CSendRequestDlg dialog


CSendRequestDlg::CSendRequestDlg(const char *name, IcqContact *c, CWnd* pParent /*=NULL*/)
: CMyDlg(CSendRequestDlg::IDD, pParent), IcqWindow(WIN_SEND_REQUEST, &c->qid)
{
	//{{AFX_DATA_INIT(CSendRequestDlg)
	m_text = _T("");
	//}}AFX_DATA_INIT

	isSend = TRUE;
	sessionName = name;
	contact = c;
	init();
}

CSendRequestDlg::CSendRequestDlg(IcqMsg *msg, CWnd *pParent)
: CMyDlg(CSendRequestDlg::IDD, pParent), IcqWindow(WIN_SEND_REQUEST, &msg->qid)
{
	TextInStream in(msg->text.c_str());
	string name, text;
	in >> name >> text >> port;

	isSend = FALSE;
	sessionName = name.c_str();
	contact = icqLink->findContact(qid);
	m_text = text.c_str();

	init();
}

void CSendRequestDlg::init()
{
	frame = 0;

	m_nick = contact->nick.c_str();
	m_qid = qid.toString();

	Create(IDD, GetDesktopWindow());
}

void CSendRequestDlg::onAck(uint32 seq)
{
	DestroyWindow();
}

void CSendRequestDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendRequestDlg)
	DDX_Control(pDX, IDC_TEXT, m_textEdit);
	DDX_Control(pDX, IDC_PIC, m_faceButton);
	DDX_Text(pDX, IDC_NICK, m_nick);
	DDX_Text(pDX, IDC_UIN, m_qid);
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendRequestDlg, CMyDlg)
	//{{AFX_MSG_MAP(CSendRequestDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendRequestDlg message handlers

BOOL CSendRequestDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	IcqPlugin *p = PluginFactory::getPlugin(string(sessionName));
	if (p) {
		CString str;
		GetWindowText(str);
		str += " ---- ";
		str += p->info.name.c_str();
		SetWindowText(str);
	}

	m_faceButton.SetIcon(getApp()->getLargeFace(contact->face));

	if (!isSend) {
		CString str;
		str.LoadString(IDS_ACCEPT_REQ);
		SetDlgItemText(IDOK, str);
		m_textEdit.SetReadOnly();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendRequestDlg::OnOK() 
{
	if (isSend) {
		UpdateData();
		seq = icqLink->sendTcpRequest(sessionName, contact, m_text);

		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_TEXT)->EnableWindow(FALSE);
		SetTimer(IDT_ANIM, 500, NULL);
	} else {
		icqLink->acceptTcpRequest(sessionName, contact, port);
		CMyDlg::OnOK();
	}
}

void CSendRequestDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_ANIM) {
		frame ^= 1;
		int status = (frame ? STATUS_AWAY : STATUS_ONLINE);
		m_faceButton.SetIcon(getApp()->getLargeFace(contact->face, status));
	} else
		CMyDlg::OnTimer(nIDEvent);
}
