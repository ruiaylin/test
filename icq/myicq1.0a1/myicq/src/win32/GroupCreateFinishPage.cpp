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

// GroupCreateFinishPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupCreateFinishPage.h"
#include "GroupCreateWizard.h"
#include "icqlink.h"
#include "udpsession.h"
#include "groupplugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateFinishPage property page

IMPLEMENT_DYNCREATE(CGroupCreateFinishPage, CPropertyPage)

CGroupCreateFinishPage::CGroupCreateFinishPage() : CPropertyPage(CGroupCreateFinishPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupCreateFinishPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGroupCreateFinishPage::~CGroupCreateFinishPage()
{
}

void CGroupCreateFinishPage::onSendError()
{
	CPropertySheet *wiz = (CPropertySheet *) GetParent();
	wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);

	CString str;
	str.LoadString(IDS_TIMEOUT);
	SetDlgItemText(IDC_STATUS, str);

	m_faceLabel.stop();
}

void CGroupCreateFinishPage::onCreateGroupReply(uint32 id)
{
	CPropertySheet *wiz = (CPropertySheet *) GetParent();

	CString str;
	if (id) {
		str.Format(IDS_CREATE_GROUP_SUCCESS, id);
		wiz->SetWizardButtons(PSWIZB_FINISH);
	} else {
		str.LoadString(IDS_CREATE_GROUP_FAILED);
		wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
	}
	SetDlgItemText(IDC_STATUS, str);

	m_faceLabel.stop();
}

void CGroupCreateFinishPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupCreateFinishPage)
	DDX_Control(pDX, IDC_FACE, m_faceLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupCreateFinishPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupCreateFinishPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateFinishPage message handlers

BOOL CGroupCreateFinishPage::OnSetActive() 
{
	CGroupCreateWizard *wiz = (CGroupCreateWizard *) GetParent();
	CString str;

	if (!GroupPlugin::get(wiz->groupPlugin)) {
		wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
		str.LoadString(IDS_GROUP_ERROR_NO_PLUGIN);

	} else {
		wiz->SetWizardButtons(PSWIZB_DISABLEDFINISH);
		str.LoadString(IDS_CREATING_GROUP);

		wiz->seq = getUdpSession()->createGroup(wiz->groupType,
			wiz->basicPage.m_groupName, wiz->basicPage.m_passwd);

		m_faceLabel.start();
	}

	SetDlgItemText(IDC_STATUS, str);

	return CPropertyPage::OnSetActive();
}

BOOL CGroupCreateFinishPage::OnWizardFinish() 
{
	GetParent()->DestroyWindow();
	return TRUE;
}
