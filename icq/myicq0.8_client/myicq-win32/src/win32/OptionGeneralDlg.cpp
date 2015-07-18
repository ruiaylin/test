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

// OptionGeneralDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "OptionGeneralDlg.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionGeneralDlg property page

IMPLEMENT_DYNCREATE(COptionGeneralDlg, CPropertyPage)

COptionGeneralDlg::COptionGeneralDlg() : CPropertyPage(COptionGeneralDlg::IDD)
{
	//{{AFX_DATA_INIT(COptionGeneralDlg)
	//}}AFX_DATA_INIT

	bitset<NR_USER_FLAGS> &f = icqLink->options.flags;

	m_alwaysOnTop = f.test(UF_ALWAYS_ON_TOP);
	m_taskbarIcon = f.test(UF_TASKBAR_ICON);
	m_showOnlineOnly = f.test(UF_SHOW_ONLINE);
	m_taskbarStyle = f.test(UF_TASKBAR_STYLE);
	m_colorfulURL = f.test(UF_COLORFUL_URL);
	m_ignoreStranger = f.test(UF_IGNORE_STRANGER);
	m_loginInvis = f.test(UF_LOGIN_INVIS);
	m_monitorNetwork = f.test(UF_MONITOR_NETWORK);
	m_onlineNotify = f.test(UF_ONLINE_NOTIFY);
	m_popupMsg = f.test(UF_AUTO_POPUP_MSG);
	m_useHotKey = f.test(UF_USE_HOTKEY);
	m_defaultHotKey = (f.test(UF_DEFAULT_HOTKEY) ? 0 : 1);
}

COptionGeneralDlg::~COptionGeneralDlg()
{
}

void COptionGeneralDlg::enableControls(BOOL enable)
{
	GetDlgItem(IDC_HOTKEY_DEFAULT)->EnableWindow(enable);
	GetDlgItem(IDC_HOTKEY_CUSTOM)->EnableWindow(enable);
	GetDlgItem(IDC_HOTKEY)->EnableWindow(enable && IsDlgButtonChecked(IDC_HOTKEY_CUSTOM));
}

void COptionGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionGeneralDlg)
	DDX_Control(pDX, IDC_HOTKEY, m_ctlHotKey);
	DDX_Check(pDX, IDC_ALWAYS_ON_TOP, m_alwaysOnTop);
	DDX_Check(pDX, IDC_TASKBAR_ICON, m_taskbarIcon);
	DDX_Check(pDX, IDC_SHOW_ONLINE_ONLY, m_showOnlineOnly);
	DDX_Check(pDX, IDC_TASKBAR_STYLE, m_taskbarStyle);
	DDX_Check(pDX, IDC_COLORFUL_URL, m_colorfulURL);
	DDX_Check(pDX, IDC_IGNORE_STRANGER, m_ignoreStranger);
	DDX_Check(pDX, IDC_LOGIN_INVIS, m_loginInvis);
	DDX_Check(pDX, IDC_MONITOR_NETWORK, m_monitorNetwork);
	DDX_Check(pDX, IDC_ONLINE_NOTIFY, m_onlineNotify);
	DDX_Check(pDX, IDC_POPUP_MSG, m_popupMsg);
	DDX_Check(pDX, IDC_USE_HOTKEY, m_useHotKey);
	DDX_Radio(pDX, IDC_HOTKEY_DEFAULT, m_defaultHotKey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionGeneralDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptionGeneralDlg)
	ON_BN_CLICKED(IDC_USE_HOTKEY, OnUseHotkey)
	ON_BN_CLICKED(IDC_HOTKEY_CUSTOM, OnHotkeyCustom)
	ON_BN_CLICKED(IDC_HOTKEY_DEFAULT, OnHotkeyDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionGeneralDlg message handlers

void COptionGeneralDlg::OnOK() 
{
	bitset<NR_USER_FLAGS> &f = icqLink->options.flags;

	f.set(UF_ALWAYS_ON_TOP, m_alwaysOnTop);
	f.set(UF_TASKBAR_STYLE, m_taskbarStyle);
	f.set(UF_ONLINE_NOTIFY, m_onlineNotify);
	f.set(UF_MONITOR_NETWORK, m_monitorNetwork);
	f.set(UF_COLORFUL_URL, m_colorfulURL);
	f.set(UF_TASKBAR_ICON, m_taskbarIcon);
	f.set(UF_SHOW_ONLINE, m_showOnlineOnly);
	f.set(UF_AUTO_POPUP_MSG, m_popupMsg);
	f.set(UF_IGNORE_STRANGER, m_ignoreStranger);
	f.set(UF_LOGIN_INVIS, m_loginInvis);
	f.set(UF_USE_HOTKEY, m_useHotKey);
	f.set(UF_DEFAULT_HOTKEY, m_defaultHotKey == 0);

	WORD vk, mod;
	m_ctlHotKey.GetHotKey(vk, mod);
	icqLink->options.hotKey = ((mod << 16) | vk);
	CPropertyPage::OnOK();
}

void COptionGeneralDlg::OnUseHotkey() 
{
	enableControls(IsDlgButtonChecked(IDC_USE_HOTKEY));
}

BOOL COptionGeneralDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	DWORD hotKey = icqLink->options.hotKey;
	m_ctlHotKey.SetHotKey(LOWORD(hotKey), HIWORD(hotKey));

	OnUseHotkey();
	GetDlgItem(IDC_HOTKEY)->EnableWindow(m_defaultHotKey == 1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionGeneralDlg::OnHotkeyCustom() 
{
	GetDlgItem(IDC_HOTKEY)->EnableWindow();
	m_ctlHotKey.SetFocus();
}

void COptionGeneralDlg::OnHotkeyDefault() 
{
	GetDlgItem(IDC_HOTKEY)->EnableWindow(FALSE);
}
