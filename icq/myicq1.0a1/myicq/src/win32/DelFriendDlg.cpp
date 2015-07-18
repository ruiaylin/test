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

// DelFriendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "DelFriendDlg.h"
#include "ViewDetailDlg.h"
#include "icqlink.h"
#include "udpsession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_ANIMATE		1001

/////////////////////////////////////////////////////////////////////////////
// CDelFriendDlg dialog


CDelFriendDlg::CDelFriendDlg(QID &qid, CWnd* pParent /*=NULL*/)
	: CMyDlg(CDelFriendDlg::IDD, pParent), IcqWindow(WIN_DEL_FRIEND, &qid)
{
	//{{AFX_DATA_INIT(CDelFriendDlg)
	//}}AFX_DATA_INIT
}

void CDelFriendDlg::onAck(uint32 seq)
{
	EndDialog(IDOK);
}

void CDelFriendDlg::onSendError(uint32 seq)
{
	GetDlgItem(IDC_DEL_FRIEND)->EnableWindow(TRUE);
	CString str;
	str.LoadString(IDS_TIMEOUT);
	SetDlgItemText(IDC_MSG, str);

	KillTimer(IDT_ANIMATE);
}

void CDelFriendDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelFriendDlg)
	DDX_Control(pDX, IDC_FACE, m_faceButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelFriendDlg, CMyDlg)
	//{{AFX_MSG_MAP(CDelFriendDlg)
	ON_BN_CLICKED(IDC_DEL_FRIEND, OnDelFriend)
	ON_BN_CLICKED(IDC_FACE, OnFace)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelFriendDlg message handlers

void CDelFriendDlg::OnDelFriend() 
{
	seq = getUdpSession()->delFriend(qid);
	GetDlgItem(IDC_DEL_FRIEND)->EnableWindow(FALSE);
	CString str;
	str.LoadString(IDS_DEL_FRIEND);
	SetDlgItemText(IDC_MSG, str);

	SetTimer(IDT_ANIMATE, 400, NULL);
}

BOOL CDelFriendDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	SetDlgItemText(IDC_UIN, qid.toString());
	IcqContact *contact = icqLink->findContact(qid);
	if (contact) {
		SetDlgItemText(IDC_NICK, contact->nick.c_str());
		m_faceButton.SetIcon(getApp()->getLargeFace(contact->face));
	}
	OnDelFriend();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDelFriendDlg::OnFace() 
{
	new CViewDetailDlg(qid, this);
}

void CDelFriendDlg::OnTimer(UINT nIDEvent) 
{	
	if (nIDEvent == IDT_ANIMATE) {
		static int frame;

		m_faceButton.SetIcon(getApp()->getLargeFace(frame));
		if (++frame >= NR_FACES)
			frame = 0;
	} else
		CMyDlg::OnTimer(nIDEvent);
}
