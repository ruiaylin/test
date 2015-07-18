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

// GroupSearchFinishPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupSearchFinishPage.h"
#include "GroupSearchWizard.h"
#include "icqlink.h"
#include "udpsession.h"
#include "groupplugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchFinishPage property page

IMPLEMENT_DYNCREATE(CGroupSearchFinishPage, CPropertyPage)

CGroupSearchFinishPage::CGroupSearchFinishPage() : CPropertyPage(CGroupSearchFinishPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupSearchFinishPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGroupSearchFinishPage::~CGroupSearchFinishPage()
{
}

void CGroupSearchFinishPage::onSendError()
{
	CGroupSearchWizard *wiz = (CGroupSearchWizard *) GetParent();
	wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);

	CString str;
	str.LoadString(IDS_TIMEOUT);
	SetDlgItemText(IDC_STATUS, str);

	m_faceLabel.stop();
}

void CGroupSearchFinishPage::onEnterGroupReply(uint32 id, uint16 error)
{
	CGroupSearchWizard *wiz = (CGroupSearchWizard *) GetParent();

	if (error == GROUP_ERROR_SUCCESS)
		wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	else
		wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
	
	CString str;

	switch (error) {
	case GROUP_ERROR_SUCCESS:
		str.Format(IDS_ENTER_GROUP_SUCCESS, id);
		break;
	case GROUP_ERROR_NOT_EXIST:
		str.Format(IDS_GROUP_NOT_EXIST);
		break;
	case GROUP_ERROR_ALREADY_EXIST:
		str.Format(IDS_GROUP_ALREADY_EXIST);
		break;
	case GROUP_ERROR_EXCEED_MAX_GROUPS:
		str.Format(IDS_GROUP_EXCEED_MAX_GROUPS);
		break;
	case GROUP_ERROR_WRONG_PASSWD:
		str.Format(IDS_WRONG_PASSWD);
		break;
	case GROUP_ERROR_EXCEED_MAX_MEMBERS:
		str.Format(IDS_GROUP_EXCEED_MAX_MEMBERS);
		break;

	default:
		str.Format(IDS_ENTER_GROUP_FAILED, id);
	}

	SetDlgItemText(IDC_STATUS, str);
	m_faceLabel.stop();
}

void CGroupSearchFinishPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupSearchFinishPage)
	DDX_Control(pDX, IDC_FACE, m_faceLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupSearchFinishPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupSearchFinishPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchFinishPage message handlers

BOOL CGroupSearchFinishPage::OnSetActive() 
{
	CGroupSearchWizard *wiz = (CGroupSearchWizard *) GetParent();
	GROUP_INFO *info = wiz->resultPage.getResult();
	GROUP_TYPE_INFO *type = icqLink->getGroupTypeInfo(info->type);
	CString str;

	if (!GroupPlugin::get(type->name.c_str())) {
		wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
		str.LoadString(IDS_GROUP_ERROR_NO_PLUGIN);

	} else {
		wiz->SetWizardButtons(PSWIZB_DISABLEDFINISH);
		str.LoadString(IDS_ENTERING_GROUP);

		wiz->seq = getUdpSession()->enterGroup(info->id, wiz->resultPage.m_passwd);

		m_faceLabel.start();
	}

	SetDlgItemText(IDC_STATUS, str);

	return CPropertyPage::OnSetActive();
}

BOOL CGroupSearchFinishPage::OnWizardFinish() 
{
	GetParent()->DestroyWindow();
	return TRUE;
}
