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

// ModifyDetailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "ModifyDetailDlg.h"
#include "icqlink.h"
#include "serversession.h"
#include "icqdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyDetailDlg

IMPLEMENT_DYNAMIC(CModifyDetailDlg, CPropertySheet)

CModifyDetailDlg::CModifyDetailDlg()
	:CPropertySheet(IDS_MODIFY_DETAIL), IcqWindow(WIN_MODIFY_DETAIL)
{
	m_psh.dwFlags |= PSH_USEHICON;
	m_psh.hIcon = getApp()->m_hIcon;
	
	AddPage(&basicPage);
	AddPage(&commPage);
	AddPage(&miscPage);
	AddPage(&authPage);
}

CModifyDetailDlg::~CModifyDetailDlg()
{
}


void CModifyDetailDlg::setData()
{
	IcqUser *user = &icqLink->myInfo;

	basicPage.m_uin = user->uin;
	basicPage.setData(user);
	commPage.setData(user);
	miscPage.setData(user);
	authPage.setData(user);
}

void CModifyDetailDlg::getData(IcqUser &user)
{
	user.nick = basicPage.m_nick;
	user.face = basicPage.m_pic;
	user.gender = basicPage.m_gender;
	user.age = basicPage.m_age;
	user.country = basicPage.m_country;
	user.province = basicPage.m_province;
	user.city = basicPage.m_city;

	user.email = commPage.m_email;
	user.address = commPage.m_address;
	user.zipcode = commPage.m_zipcode;
	user.tel = commPage.m_tel;

	user.name = miscPage.m_name;
	user.blood = miscPage.m_blood;
	user.college = miscPage.m_college;
	user.profession = miscPage.m_profession;
	user.intro = miscPage.m_intro;

	user.auth = authPage.m_auth;
	if (authPage.m_modifyPasswd)
		user.passwd = authPage.m_passwd;
}

void CModifyDetailDlg::enableAll(BOOL enable)
{
	basicPage.enableAll(enable);
	commPage.enableAll(enable);
	miscPage.enableAll(enable);
	authPage.enableAll(enable);

	basicPage.GetDlgItem(IDC_UPDATE)->EnableWindow(enable);
	commPage.GetDlgItem(IDC_UPDATE)->EnableWindow(enable);
	miscPage.GetDlgItem(IDC_UPDATE)->EnableWindow(enable);
	authPage.GetDlgItem(IDC_UPDATE)->EnableWindow(enable);
}

void CModifyDetailDlg::onAck(uint32 seq)
{
	enableAll();
	getData(icqLink->myInfo);
	IcqDB::saveUser(icqLink->myInfo);

	myMessageBox(IDS_SUCCESS_MODIFY_USER, IDS_SUCCESS, this, MB_OK | MB_ICONINFORMATION);
	DestroyWindow();
}

void CModifyDetailDlg::onSendError(uint32 seq)
{
	myMessageBox(IDS_TIMEOUT_RETRY, IDS_SORRY, this, MB_OK | MB_ICONERROR);
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(CModifyDetailDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CModifyDetailDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_UPDATE, OnModify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyDetailDlg message handlers

void CModifyDetailDlg::PostNcDestroy() 
{
	delete this;
}

BOOL CModifyDetailDlg::Create(CWnd *pParentWnd)
{
	DWORD dwStyle = WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME;
	BOOL res = CPropertySheet::Create(pParentWnd, dwStyle);

	int n = GetPageCount();
	while (--n >= 0)
		SetActivePage(n);

	setData();

	basicPage.GetDlgItem(IDC_REMARK)->ShowWindow(SW_HIDE);
	commPage.GetDlgItem(IDC_REMARK)->ShowWindow(SW_HIDE);
	miscPage.GetDlgItem(IDC_REMARK)->ShowWindow(SW_HIDE);

	ShowWindow(SW_NORMAL);
	return res;
}

void CModifyDetailDlg::OnModify()
{
	if (!basicPage.UpdateData() ||
		!commPage.UpdateData() ||
		!miscPage.UpdateData() ||
		!authPage.UpdateData())
		return;

	enableAll(FALSE);

	IcqUser user;
	getData(user);
	seq = serverSession()->modifyUser(&user, authPage.m_modifyPasswd);
}
