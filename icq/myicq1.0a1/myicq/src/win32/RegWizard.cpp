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

// RegWizard.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RegWizard.h"
#include "icqclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegWizard

IMPLEMENT_DYNAMIC(CRegWizard, CPropertySheet)

CRegWizard::CRegWizard() : IcqWindow(WIN_REG_WIZARD)
{
	isFinished = FALSE;
	
	SetWizardMode();
	m_psh.dwFlags |= PSH_USEHICON;
	m_psh.hIcon = getApp()->m_hIcon;

	AddPage(&modeDlg);
	AddPage(&basicDlg);
	AddPage(&commDlg);
	AddPage(&miscDlg);
	AddPage(&networkDlg);
	AddPage(&finishDlg);
}

CRegWizard::~CRegWizard()
{
}

void CRegWizard::getData(IcqUser *info, IcqOption *options)
{
	if (modeDlg.m_mode == 0)
		info->qid = qid;
	else {
		info->qid.uin = modeDlg.m_uin;
		info->qid.domain = finishDlg.domain;
	}

	info->face = basicDlg.m_pic;
	info->age = basicDlg.m_age;
	info->nick = basicDlg.m_nick;
	info->gender = basicDlg.m_gender;
	info->country = basicDlg.m_country;
	info->province = basicDlg.m_province;
	info->city = basicDlg.m_city;

	info->email = commDlg.m_email;
	info->address = commDlg.m_address;
	info->zipcode = commDlg.m_zipcode;
	info->tel = commDlg.m_tel;

	info->name = miscDlg.m_name;
	info->blood = miscDlg.m_blood;
	info->college = miscDlg.m_college;
	info->profession = miscDlg.m_profession;
	info->homepage = miscDlg.m_homepage;
	info->intro = miscDlg.m_intro;

	info->passwd = (modeDlg.m_mode == 0 ? basicDlg.m_passwd : modeDlg.m_passwd);

	if (options) {
		options->host = networkDlg.m_host;
		options->port = networkDlg.m_port;
		options->flags.set(UF_USE_PROXY, networkDlg.m_useProxy);
		options->proxyType = networkDlg.m_proxyType;
		for (int i = 0; i < NR_PROXY_TYPES; ++i)
			options->proxy[i] = networkDlg.proxyInfo[i];
	}
}


BEGIN_MESSAGE_MAP(CRegWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CRegWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegWizard message handlers

