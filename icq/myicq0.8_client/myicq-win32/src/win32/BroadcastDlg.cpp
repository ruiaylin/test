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

// BroadcastDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "BroadcastDlg.h"
#include "serversession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBroadcastDlg dialog

CBroadcastDlg::CBroadcastDlg(CWnd* pParent /*=NULL*/)
	: CMyDlg(CBroadcastDlg::IDD, pParent), IcqWindow(WIN_BROADCAST_MSG)
{
	//{{AFX_DATA_INIT(CBroadcastDlg)
	m_text = _T("");
	//}}AFX_DATA_INIT

	m_expire = CTime::GetCurrentTime();
}

void CBroadcastDlg::onAck(uint32 seq)
{
	DestroyWindow();
}

void CBroadcastDlg::onSendError(uint32 seq)
{
	myMessageBox(IDS_TIMEOUT, IDS_FAILED, this, MB_OK | MB_ICONERROR);
	enableControls(TRUE);
}

void CBroadcastDlg::enableControls(BOOL enable)
{
	GetDlgItem(IDC_EXPIRE)->EnableWindow(enable);
	m_textEdit.SetReadOnly(!enable);
	GetDlgItem(IDC_SEND)->EnableWindow(enable);
}

void CBroadcastDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBroadcastDlg)
	DDX_Control(pDX, IDC_TEXT, m_textEdit);
	DDX_MonthCalCtrl(pDX, IDC_EXPIRE, m_expire);
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBroadcastDlg, CMyDlg)
	//{{AFX_MSG_MAP(CBroadcastDlg)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBroadcastDlg message handlers

void CBroadcastDlg::PostNcDestroy() 
{
	delete this;
}

void CBroadcastDlg::OnSend() 
{
	UpdateData();

	seq = serverSession()->broadcastMsg(MSG_TEXT, m_expire.GetTime(), m_text);

	enableControls(FALSE);
}
