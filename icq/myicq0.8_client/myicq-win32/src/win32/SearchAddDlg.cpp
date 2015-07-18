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
#include "serversession.h"
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
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSearchAddDlg::~CSearchAddDlg()
{
}

void CSearchAddDlg::onAddFriendReply(uint8 result)
{
	CString str;

	if (result == ADD_FRIEND_REJECTED) {
		str.LoadString(IDS_ADD_REJECTED);
		SetDlgItemText(IDC_STATUS_DETAIL, str);

	} else if (result == ADD_FRIEND_ACCEPTED) {
		str.LoadString(IDS_ADD_ACCEPTED);
		SetDlgItemText(IDC_STATUS_DETAIL, str);
		((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	} else if (result == ADD_FRIEND_AUTH_REQ) {
		str.LoadString(IDS_AUTH_REQUIRED);
		SetDlgItemText(IDC_STATUS_DETAIL, str);
		GetDlgItem(IDC_REQUEST)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_SEND_REQUEST)->ShowWindow(SW_NORMAL);
	}
	m_ctlAnim.stop();
}

void CSearchAddDlg::onAck(uint32 seq)
{
	m_ctlAnim.stop();
	CString str;
	str.LoadString(IDS_REQUEST_SENT);
	SetDlgItemText(IDC_STATUS_DETAIL, str);
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
}

void CSearchAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchAddDlg)
	DDX_Control(pDX, IDC_STATIC_PIC, m_ctlAnim);
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
	if (!result)
		return FALSE;

	wiz->uin = result->uin;
	SetDlgItemInt(IDC_UIN, result->uin, FALSE);
	SetDlgItemText(IDC_NICK, result->nick.c_str());
	SetDlgItemText(IDC_PROVINCE, result->province.c_str());

	wiz->seq = serverSession()->addFriend(result->uin);
	m_ctlAnim.start();
	
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
	wiz->seq = serverSession()->sendMessage(MSG_AUTH_REQUEST, wiz->uin, out);

	GetDlgItem(IDC_REQUEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND_REQUEST)->EnableWindow(FALSE);
	m_ctlAnim.start();
}

BOOL CSearchAddDlg::OnWizardFinish() 
{
	GetParent()->DestroyWindow();
	return CPropertyPage::OnWizardFinish();
}
