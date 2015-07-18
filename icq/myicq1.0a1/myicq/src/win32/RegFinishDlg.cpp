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

// RegFinishDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RegFinishDlg.h"
#include "RegWizard.h"
#include "myicqDlg.h"
#include "udpsession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegFinishDlg property page

IMPLEMENT_DYNCREATE(CRegFinishDlg, CPropertyPage)

CRegFinishDlg::CRegFinishDlg() : CPropertyPage(CRegFinishDlg::IDD)
{
	//{{AFX_DATA_INIT(CRegFinishDlg)
	//}}AFX_DATA_INIT
}

CRegFinishDlg::~CRegFinishDlg()
{
}

void CRegFinishDlg::onSendError(uint32 seq)
{
	CRegWizard *wiz = (CRegWizard *) GetParent();

	if (!wiz->isFinished)
		onTimeout();
	else
		onSuccess();
}

void CRegFinishDlg::onTimeout()
{
	CString str;
	str.LoadString(IDS_FAILED);
	SetDlgItemText(IDC_STATUS, str);
	str.LoadString(IDS_TIMEOUT);
	SetDlgItemText(IDC_STATUS_DETAIL, str);

	CRegWizard *wiz = (CRegWizard *) GetParent();
	wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);

	m_faceLabel.stop();
}

void CRegFinishDlg::onSuccess()
{
	CString str;
	str.LoadString(IDS_FINISHED);
	SetDlgItemText(IDC_STATUS, str);
	str.LoadString(IDS_SUCCESS_CONTACTLIST);
	SetDlgItemText(IDC_STATUS_DETAIL, str);

	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_FINISH);
	m_faceLabel.stop();
}

void CRegFinishDlg::onNewUINReply(QID &qid)
{
	CRegWizard *wiz = (CRegWizard *) GetParent();
	wiz->qid = qid;

	CString str;

	if (qid.uin) {
		wiz->isFinished = TRUE;

		str.LoadString(IDS_FINISHED);
		SetDlgItemText(IDC_STATUS, str);
		str.Format(IDS_REG_SUCCESS, qid.toString());
		SetDlgItemText(IDC_STATUS_DETAIL, str);

		wiz->SetWizardButtons(PSWIZB_FINISH);
		wiz->GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

	} else {
		str.LoadString(IDS_FAILED);
		SetDlgItemText(IDC_STATUS, str);
		str.LoadString(IDS_REG_FAILED);
		SetDlgItemText(IDC_STATUS_DETAIL, str);
	}

	m_faceLabel.stop();
}

void CRegFinishDlg::onContactListReply()
{
	CRegWizard *wiz = (CRegWizard *) GetParent();
	wiz->isFinished = TRUE;

	wiz->seq = getUdpSession()->getRemoteContactList();
}

void CRegFinishDlg::onRemoteContactList(const char *domain)
{
	CRegWizard *wiz = (CRegWizard *) GetParent();
	this->domain = domain;

	onSuccess();

	getUdpSession()->logout();
}

void CRegFinishDlg::onLoginReply(uint8 error)
{
	CRegWizard *wiz = (CRegWizard *) GetParent();

	if (error == LOGIN_SUCCESS)
		wiz->seq = getUdpSession()->getContactList();
	else {
		CString str;
		str.LoadString(IDS_FAILED);
		SetDlgItemText(IDC_STATUS, str);
		if (error == LOGIN_WRONG_PASSWD)
			str.LoadString(IDS_WRONG_PASSWD);
		else
			str.LoadString(IDS_INVALID_UIN);
		SetDlgItemText(IDC_STATUS_DETAIL, str);

		wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
		m_faceLabel.stop();
	}
}

void CRegFinishDlg::onConnect(bool success)
{
	if (success) {
		CRegWizard *wiz = (CRegWizard *) GetParent();

		if (wiz->modeDlg.m_mode == 0)
			wiz->seq = getUdpSession()->regNewUIN(wiz->basicDlg.m_passwd);
		else {
			icqLink->myInfo.qid.uin = wiz->modeDlg.m_uin;
			wiz->seq = getUdpSession()->login(wiz->modeDlg.m_passwd, STATUS_OFFLINE);
		}
	} else
		onTimeout();
}

void CRegFinishDlg::onHostFound(struct in_addr &addr)
{
	if (addr.s_addr != INADDR_NONE) {
		CRegWizard *wiz = (CRegWizard *) GetParent();
		getUdpSession()->connect(inet_ntoa(addr), wiz->networkDlg.m_port);
	} else
		onTimeout();
}

void CRegFinishDlg::resolveHost()
{
	CRegWizard *wiz = (CRegWizard *) GetParent();
	const char *host = wiz->networkDlg.m_host;

	if (wiz->networkDlg.m_useProxy && !wiz->networkDlg.m_proxyResolve)
		getUdpSession()->connect(host, wiz->networkDlg.m_port);
	else {
		struct in_addr addr;
		addr.s_addr = inet_addr(host);
		if (addr.s_addr != INADDR_NONE)
			onHostFound(addr);
		else
			((CIcqDlg *) AfxGetMainWnd())->getHostByName(host);
	}
}

void CRegFinishDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegFinishDlg)
	DDX_Control(pDX, IDC_FACE, m_faceLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegFinishDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CRegFinishDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegFinishDlg message handlers

BOOL CRegFinishDlg::OnSetActive() 
{
	CRegWizard *wiz = (CRegWizard *) GetParent();
	wiz->getData(&icqLink->myInfo, &icqLink->options);
	wiz->qid.uin = 0;

	CString str;
	str.LoadString(IDS_PLEASE_WAIT);
	SetDlgItemText(IDC_STATUS, str);
	str.LoadString(IDS_REG_REGISTERING);
	SetDlgItemText(IDC_STATUS_DETAIL, str);
	wiz->SetWizardButtons(PSWIZB_DISABLEDFINISH);

	m_faceLabel.start();

	resolveHost();

	return CPropertyPage::OnSetActive();
}
