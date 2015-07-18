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

// SearchAddDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SearchAddDlg.h"
#include "SearchWizard.h"
#include "udpsession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchAddDlg property page

IMPLEMENT_DYNCREATE(CSearchAddDlg, CPropertyPage)

CSearchAddDlg::CSearchAddDlg() : CPropertyPage(CSearchAddDlg::IDD)
{
	//{{AFX_DATA_INIT(CSearchAddDlg)
	//}}AFX_DATA_INIT
}

CSearchAddDlg::~CSearchAddDlg()
{
}

void CSearchAddDlg::onAddFriendReply(uint8 auth)
{
	CString str;
	CPropertySheet *wiz = (CPropertySheet *) GetParent();

	switch (auth) {
	case ADD_FRIEND_REJECTED:
		str.LoadString(IDS_ADD_REJECTED);
		SetDlgItemText(IDC_STATUS_DETAIL, str);
		wiz->SetWizardButtons(PSWIZB_BACK);
		break;

	case ADD_FRIEND_ACCEPTED:
		str.LoadString(IDS_ADD_ACCEPTED);
		SetDlgItemText(IDC_STATUS_DETAIL, str);
		wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
		break;

	case ADD_FRIEND_AUTH_REQ:
		str.LoadString(IDS_AUTH_REQUIRED);
		SetDlgItemText(IDC_STATUS_DETAIL, str);

		GetDlgItem(IDC_SEND_REQUEST)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_REQUEST)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_REQUEST)->SetFocus();

		wiz->SetWizardButtons(PSWIZB_BACK);
		break;
	}

	m_faceLabel.stop();
}

void CSearchAddDlg::onAck(uint32 seq)
{
	m_faceLabel.stop();

	CString str;
	str.LoadString(IDS_REQUEST_SENT);
	SetDlgItemText(IDC_STATUS_DETAIL, str);
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
}

void CSearchAddDlg::onSendError(uint32 seq)
{
	m_faceLabel.stop();

	CString str;
	str.LoadString(IDS_TIMEOUT);
	SetDlgItemText(IDC_STATUS_DETAIL, str);

	GetDlgItem(IDC_REQUEST)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEND_REQUEST)->EnableWindow(TRUE);
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK);
}

void CSearchAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchAddDlg)
	DDX_Control(pDX, IDC_FACE, m_faceLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchAddDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSearchAddDlg)
	ON_BN_CLICKED(IDC_SEND_REQUEST, OnSendRequest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchAddDlg message handlers

BOOL CSearchAddDlg::OnSetActive() 
{
	CSearchWizard *wiz = (CSearchWizard *) GetParent();

	GetDlgItem(IDC_REQUEST)->EnableWindow();
	GetDlgItem(IDC_REQUEST)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SEND_REQUEST)->EnableWindow();
	GetDlgItem(IDC_SEND_REQUEST)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDC_REQUEST, "");
	wiz->SetWizardButtons(PSWIZB_DISABLEDFINISH);

	SEARCH_RESULT *result = wiz->resultDlg.getResult();
	SetDlgItemInt(IDC_UIN, result->uin, FALSE);
	SetDlgItemText(IDC_NICK, result->nick.c_str());
	SetDlgItemText(IDC_PROVINCE, result->province.c_str());

	wiz->seq = getUdpSession()->addFriend(wiz->qid);
	m_faceLabel.start();

	return CPropertyPage::OnSetActive();
}

void CSearchAddDlg::OnSendRequest() 
{
	CString str;
	GetDlgItemText(IDC_REQUEST, str);

	IcqUser &info = icqLink->myInfo;
	TextOutStream out;
	out << info.face << info.nick.c_str() << str;
	CSearchWizard *wiz = (CSearchWizard *) GetParent();
	wiz->seq = getUdpSession()->sendMessage(MSG_AUTH_REQUEST, wiz->qid, out);

	GetDlgItem(IDC_REQUEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND_REQUEST)->EnableWindow(FALSE);
	m_faceLabel.start();
}

BOOL CSearchAddDlg::OnWizardFinish() 
{
	GetParent()->DestroyWindow();
	return TRUE;
}
