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
#include "serversession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelFriendDlg dialog


CDelFriendDlg::CDelFriendDlg(uint32 uin, CWnd* pParent /*=NULL*/)
	: CMyDlg(CDelFriendDlg::IDD, pParent), IcqWindow(WIN_DEL_FRIEND, uin)
{
	//{{AFX_DATA_INIT(CDelFriendDlg)
		// NOTE: the ClassWizard will add member initialization here
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
	m_btnPic.stop();
}

void CDelFriendDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelFriendDlg)
	DDX_Control(pDX, IDC_PIC, m_btnPic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelFriendDlg, CMyDlg)
	//{{AFX_MSG_MAP(CDelFriendDlg)
	ON_BN_CLICKED(IDC_DEL_FRIEND, OnDelFriend)
	ON_BN_CLICKED(IDC_PIC, OnPic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelFriendDlg message handlers

void CDelFriendDlg::OnDelFriend() 
{
	seq = serverSession()->delFriend(uin);
	GetDlgItem(IDC_DEL_FRIEND)->EnableWindow(FALSE);
	CString str;
	str.LoadString(IDS_DEL_FRIEND);
	SetDlgItemText(IDC_MSG, str);
	m_btnPic.start();
}

BOOL CDelFriendDlg::OnInitDialog() 
{
	CMyDlg::OnInitDialog();

	SetDlgItemInt(IDC_UIN, uin, FALSE);
	IcqContact *contact = icqLink->findContact(uin);
	if (contact)
		SetDlgItemText(IDC_NICK, contact->nick.c_str());
	
	m_btnPic.setImageList(&getApp()->largeImageList, getApp()->nrFaces);

	OnDelFriend();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDelFriendDlg::OnPic() 
{
	CViewDetailDlg *win = new CViewDetailDlg(uin);
	win->Create(this);
}
