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

// RegNetworkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RegNetworkDlg.h"
#include "RegWizard.h"
#include "icqclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MYICQ_PORT_DEFAULT		8000
#define SOCKS_PORT_DEFAULT		1080
#define HTTP_PROXY_PORT_DEFAULT	8080

/////////////////////////////////////////////////////////////////////////////
// CRegNetworkDlg property page

IMPLEMENT_DYNCREATE(CRegNetworkDlg, CPropertyPage)

CRegNetworkDlg::CRegNetworkDlg() : CPropertyPage(CRegNetworkDlg::IDD)
{
	//{{AFX_DATA_INIT(CRegNetworkDlg)
	m_proxyHost = _T("");
	m_proxyPasswd = _T("");
	m_proxyPort = 0;
	m_useProxy = FALSE;
	m_proxyUserName = _T("");
	m_proxyResolve = FALSE;
	m_proxyType = -1;
	//}}AFX_DATA_INIT

	m_port = MYICQ_PORT_DEFAULT;

	proxyInfo[PROXY_SOCKS].port = SOCKS_PORT_DEFAULT;
	proxyInfo[PROXY_HTTP].port = HTTP_PROXY_PORT_DEFAULT;

	socksProxy.setListener(this);
	httpProxy.setListener(this);
}

CRegNetworkDlg::~CRegNetworkDlg()
{
}

void CRegNetworkDlg::onEstablished(bool success)
{
	if (success)
		myMessageBox(IDS_PROXY_SUCCESS, IDS_SUCCESS, this, MB_ICONINFORMATION);
	else
		myMessageBox(IDS_PROXY_FAILED, IDS_FAILED, this, MB_ICONERROR);
}

void CRegNetworkDlg::getProxyInfo(ProxyInfo &proxy)
{
	proxy.host = m_proxyHost;
	proxy.port = m_proxyPort;
	proxy.username = m_proxyUserName;
	proxy.passwd = m_proxyPasswd;
}

void CRegNetworkDlg::enableControls()
{
	BOOL b = IsDlgButtonChecked(IDC_USE_PROXY);

	GetDlgItem(IDC_PROXY_TYPE)->EnableWindow(b);
	GetDlgItem(IDC_PROXY_HOST)->EnableWindow(b);
	GetDlgItem(IDC_PROXY_PORT)->EnableWindow(b);
	GetDlgItem(IDC_PROXY_USERNAME)->EnableWindow(b);
	GetDlgItem(IDC_PROXY_PASSWD)->EnableWindow(b);
	GetDlgItem(IDC_PROXY_TEST)->EnableWindow(b);
	GetDlgItem(IDC_PROXY_RESOLVE)->EnableWindow(b);
}

void CRegNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegNetworkDlg)
	DDX_Control(pDX, IDC_HOST, m_hostCombo);
	DDX_Control(pDX, IDC_PROXY_TYPE, m_proxyTypeCombo);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Text(pDX, IDC_PROXY_HOST, m_proxyHost);
	DDX_Text(pDX, IDC_PROXY_PASSWD, m_proxyPasswd);
	DDX_Text(pDX, IDC_PROXY_PORT, m_proxyPort);
	DDX_Check(pDX, IDC_USE_PROXY, m_useProxy);
	DDX_Text(pDX, IDC_PROXY_USERNAME, m_proxyUserName);
	DDX_Check(pDX, IDC_PROXY_RESOLVE, m_proxyResolve);
	DDX_CBIndex(pDX, IDC_PROXY_TYPE, m_proxyType);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
		DDX_CBString(pDX, IDC_HOST, m_host);
}


BEGIN_MESSAGE_MAP(CRegNetworkDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CRegNetworkDlg)
	ON_BN_CLICKED(IDC_PROXY_TEST, OnProxyTest)
	ON_BN_CLICKED(IDC_USE_PROXY, OnUseProxy)
	ON_CBN_SELCHANGE(IDC_PROXY_TYPE, OnSelchangeProxyType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegNetworkDlg message handlers

BOOL CRegNetworkDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	getApp()->loadText(m_hostCombo, DATA_HOST);
	m_hostCombo.SetCurSel(0);

	m_proxyTypeCombo.SetCurSel(0);
	OnSelchangeProxyType();
	enableControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CRegNetworkDlg::OnWizardBack() 
{
	if (((CRegWizard *) GetParent())->modeDlg.m_mode == 1)
		return IDD_REG_MODE;
	return CPropertyPage::OnWizardBack();
}

BOOL CRegNetworkDlg::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}

void CRegNetworkDlg::OnProxyTest() 
{
	ProxyInfo proxy;

	UpdateData();
	getProxyInfo(proxy);

	if (m_proxyType == PROXY_SOCKS)
		socksProxy.start(proxy);
	else if (m_proxyType == PROXY_HTTP)
		httpProxy.start(m_host, proxy);
}

void CRegNetworkDlg::OnUseProxy() 
{
	enableControls();
}

void CRegNetworkDlg::OnSelchangeProxyType() 
{
	if (m_proxyType >= 0) {
		int oldType = m_proxyType;
		UpdateData();
		getProxyInfo(proxyInfo[oldType]);
	}

	m_proxyType = m_proxyTypeCombo.GetCurSel();
	ProxyInfo &proxy = proxyInfo[m_proxyType];
	m_proxyHost = proxy.host.c_str();
	m_proxyPort = proxy.port;
	m_proxyUserName = proxy.username.c_str();
	m_proxyPasswd = proxy.passwd.c_str();
	m_proxyResolve = proxy.resolve;
	UpdateData(FALSE);
}

BOOL CRegNetworkDlg::OnKillActive() 
{
	if (!CPropertyPage::OnKillActive())
		return FALSE;

	getProxyInfo(proxyInfo[m_proxyType]);	
	return TRUE;
}
