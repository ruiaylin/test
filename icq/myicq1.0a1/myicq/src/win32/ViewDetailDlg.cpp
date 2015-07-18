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

// ViewDetailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "ViewDetailDlg.h"
#include "udpsession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewDetailDlg

IMPLEMENT_DYNAMIC(CViewDetailDlg, CPropertySheet)

CViewDetailDlg::CViewDetailDlg(QID &qid, CWnd *parent)
: CPropertySheet(IDS_VIEW_DETAIL), IcqWindow(WIN_VIEW_DETAIL, &qid)
{
	contact = icqLink->findContact(qid);
	
	m_psh.dwFlags |= PSH_USEHICON;
	m_psh.hIcon = getApp()->m_hIcon;

	basicPage.m_qid = qid.toString();

	AddPage(&basicPage);
	AddPage(&commPage);
	AddPage(&miscPage);

	Create(parent);
}

CViewDetailDlg::~CViewDetailDlg()
{
}

void CViewDetailDlg::enableButtons(BOOL enable)
{
	basicPage.GetDlgItem(IDC_REMARK)->EnableWindow(enable);
	basicPage.GetDlgItem(IDC_UPDATE)->EnableWindow(enable);
	commPage.GetDlgItem(IDC_REMARK)->EnableWindow(enable);
	commPage.GetDlgItem(IDC_UPDATE)->EnableWindow(enable);
	miscPage.GetDlgItem(IDC_REMARK)->EnableWindow(enable);
	miscPage.GetDlgItem(IDC_UPDATE)->EnableWindow(enable);
}

void CViewDetailDlg::setData(IcqContact *c)
{
	basicPage.setData(c);
	commPage.setData(c);
	miscPage.setData(c);
}

void CViewDetailDlg::onUpdateContactReply(IcqContact *c)
{
	setData(c);
	enableButtons(TRUE);
}

void CViewDetailDlg::onSendError(uint32 seq)
{
	myMessageBox(IDS_TIMEOUT_RETRY, IDS_SORRY, this, MB_OK | MB_ICONERROR);
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CViewDetailDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CViewDetailDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_UPDATE, OnUpdate)
END_MESSAGE_MAP()


BOOL CViewDetailDlg::Create(CWnd *pParentWnd)
{
	DWORD dwStyle = WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_MINIMIZEBOX;
	BOOL res = CPropertySheet::Create(pParentWnd, dwStyle);

	int n = GetPageCount();
	while (--n >= 0)
		SetActivePage(n);

	basicPage.enableAll(FALSE);
	commPage.enableAll(FALSE);
	miscPage.enableAll(FALSE);

	if (!contact) {
		basicPage.GetDlgItem(IDC_REMARK)->ShowWindow(SW_HIDE);
		basicPage.GetDlgItem(IDC_UPDATE)->ShowWindow(SW_HIDE);
		basicPage.GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
		commPage.GetDlgItem(IDC_REMARK)->ShowWindow(SW_HIDE);
		commPage.GetDlgItem(IDC_UPDATE)->ShowWindow(SW_HIDE);
		commPage.GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
		miscPage.GetDlgItem(IDC_REMARK)->ShowWindow(SW_HIDE);
		miscPage.GetDlgItem(IDC_UPDATE)->ShowWindow(SW_HIDE);
		miscPage.GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

		OnUpdate();
	} else {
		AddPage(&customPage);

		setData(contact);
	
		CString str;
		str.LoadString(IDS_UPDATE);
		basicPage.GetDlgItem(IDC_UPDATE)->SetWindowText(str);
		commPage.GetDlgItem(IDC_UPDATE)->SetWindowText(str);
		miscPage.GetDlgItem(IDC_UPDATE)->SetWindowText(str);
	}

	ShowWindow(SW_NORMAL);
	return res;
}

/////////////////////////////////////////////////////////////////////////////
// CViewDetailDlg message handlers

void CViewDetailDlg::OnUpdate()
{
	seq = getUdpSession()->updateContact(qid);
	enableButtons(FALSE);
}

void CViewDetailDlg::PostNcDestroy() 
{
	delete this;
}
