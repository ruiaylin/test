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

// OptionNetworkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "OptionNetworkDlg.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionNetworkDlg property page

IMPLEMENT_DYNCREATE(COptionNetworkDlg, CPropertyPage)

COptionNetworkDlg::COptionNetworkDlg() : CPropertyPage(COptionNetworkDlg::IDD)
{
	//{{AFX_DATA_INIT(COptionNetworkDlg)
	m_host = _T("");
	m_port = 0;
	m_proxyHost = _T("");
	m_proxyPasswd = _T("");
	m_proxyUserName = _T("");
	m_proxyPort = 0;
	m_proxyType = -1;
	m_proxyResolve = FALSE;
	//}}AFX_DATA_INIT

	socksSession.setListener(this);
	httpSession.setListener(this);

	IcqOption &options = icqLink->options;
	m_host = options.host.c_str();
	m_port = options.port;
	m_useProxy = options.flags.test(UF_USE_PROXY);
}

COptionNetworkDlg::~COptionNetworkDlg()
{
}

void COptionNetworkDlg::getProxyInfo(ProxyInfo &proxy)
{
	proxy.host = m_proxyHost;
	proxy.port = m_proxyPort;
	proxy.username = m_proxyUserName;
	proxy.passwd = m_proxyPasswd;
	proxy.resolve = m_proxyResolve;
}

void COptionNetworkDlg::enableControls()
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

void COptionNetworkDlg::onEstablished(bool success)
{
	if (success)
		myMessageBox(IDS_PROXY_SUCCESS, IDS_SUCCESS, this, MB_ICONINFORMATION);
	else
		myMessageBox(IDS_PROXY_FAILED, IDS_FAILED, this, MB_ICONERROR);
}

void COptionNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionNetworkDlg)
	DDX_Control(pDX, IDC_PROXY_TYPE, m_proxyTypeCombo);
	DDX_Text(pDX, IDC_HOST, m_host);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Check(pDX, IDC_USE_PROXY, m_useProxy);
	DDX_Text(pDX, IDC_PROXY_HOST, m_proxyHost);
	DDX_Text(pDX, IDC_PROXY_PASSWD, m_proxyPasswd);
	DDX_Text(pDX, IDC_PROXY_USERNAME, m_proxyUserName);
	DDX_Text(pDX, IDC_PROXY_PORT, m_proxyPort);
	DDX_CBIndex(pDX, IDC_PROXY_TYPE, m_proxyType);
	DDX_Check(pDX, IDC_PROXY_RESOLVE, m_proxyResolve);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionNetworkDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptionNetworkDlg)
	ON_BN_CLICKED(IDC_USE_PROXY, OnUseProxy)
	ON_BN_CLICKED(IDC_PROXY_TEST, OnProxyTest)
	ON_CBN_SELCHANGE(IDC_PROXY_TYPE, OnSelchangeProxyType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionNetworkDlg message handlers

BOOL COptionNetworkDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_proxyTypeCombo.SetCurSel(icqLink->options.proxyType);
	OnSelchangeProxyType();
	enableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionNetworkDlg::OnOK() 
{
	IcqOption &options = icqLink->options;

	options.host = m_host;
	options.port = m_port;

	options.flags.set(UF_USE_PROXY, m_useProxy);
	options.proxyType = m_proxyType;
	getProxyInfo(options.proxy[m_proxyType]);

	CPropertyPage::OnOK();
}

void COptionNetworkDlg::OnUseProxy() 
{
	enableControls();
}

void COptionNetworkDlg::OnProxyTest() 
{
	ProxyInfo proxy;

	UpdateData();
	getProxyInfo(proxy);

	switch (m_proxyType) {
	case PROXY_SOCKS:
		socksSession.start(proxy);
		break;
	case PROXY_HTTP:
		httpSession.start(m_host, proxy);
		break;
	}
}

void COptionNetworkDlg::OnSelchangeProxyType() 
{
	ProxyInfo *proxy = icqLink->options.proxy;

	if (m_proxyType >= 0) {
		int oldType = m_proxyType;
		UpdateData();
		getProxyInfo(proxy[oldType]);
	}

	m_proxyType = m_proxyTypeCombo.GetCurSel();
	proxy += m_proxyType;
	m_proxyHost = proxy->host.c_str();
	m_proxyPort = proxy->port;
	m_proxyUserName = proxy->username.c_str();
	m_proxyPasswd = proxy->passwd.c_str();
	m_proxyResolve = proxy->resolve;
	UpdateData(FALSE);
}
