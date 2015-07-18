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

#if !defined(AFX_REGNETWORKDLG_H__CE7B4479_66E9_4564_BC6C_B0DEF9E98977__INCLUDED_)
#define AFX_REGNETWORKDLG_H__CE7B4479_66E9_4564_BC6C_B0DEF9E98977__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegNetworkDlg.h : header file
//

#include "proxy.h"
#include "socksproxy.h"
#include "httpproxy.h"
#include "icqclient.h"


/////////////////////////////////////////////////////////////////////////////
// CRegNetworkDlg dialog

class CRegNetworkDlg : public CPropertyPage, public ProxyListener
{
	DECLARE_DYNCREATE(CRegNetworkDlg)

// Construction
public:
	CRegNetworkDlg();
	~CRegNetworkDlg();

	void getProxyInfo(ProxyInfo &proxy);

	ProxyInfo proxyInfo[NR_PROXY_TYPES];


// Dialog Data
	//{{AFX_DATA(CRegNetworkDlg)
	enum { IDD = IDD_REG_NETWORK };
	CComboBox	m_hostCombo;
	CComboBox	m_proxyTypeCombo;
	int		m_port;
	CString	m_proxyHost;
	CString	m_proxyPasswd;
	int		m_proxyPort;
	BOOL	m_useProxy;
	CString	m_proxyUserName;
	BOOL	m_proxyResolve;
	int		m_proxyType;
	//}}AFX_DATA

	CString m_host;

private:
	virtual void onEstablished(bool success);
	void enableControls();

	SocksProxy socksProxy;
	HttpProxy httpProxy;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegNetworkDlg)
	public:
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRegNetworkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnProxyTest();
	afx_msg void OnUseProxy();
	afx_msg void OnConnectProxy();
	afx_msg void OnSelchangeProxyType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGNETWORKDLG_H__CE7B4479_66E9_4564_BC6C_B0DEF9E98977__INCLUDED_)
