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

#if !defined(AFX_OPTIONNETWORKDLG_H__B2F02B53_1E78_4A26_A4F1_1AAB8AA36D15__INCLUDED_)
#define AFX_OPTIONNETWORKDLG_H__B2F02B53_1E78_4A26_A4F1_1AAB8AA36D15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionNetworkDlg.h : header file
//

#include "sessionlistener.h"
#include "sockssession.h"
#include "httpsession.h"

class ProxyInfo;

/////////////////////////////////////////////////////////////////////////////
// COptionNetworkDlg dialog

class COptionNetworkDlg : public CPropertyPage, public SessionListener
{
	DECLARE_DYNCREATE(COptionNetworkDlg)

// Construction
public:
	COptionNetworkDlg();
	~COptionNetworkDlg();

// Dialog Data
	//{{AFX_DATA(COptionNetworkDlg)
	enum { IDD = IDD_OPTION_NETWORK };
	CComboBox	m_proxyTypeCombo;
	CString	m_host;
	int		m_port;
	BOOL	m_useProxy;
	CString	m_proxyHost;
	CString	m_proxyPasswd;
	CString	m_proxyUserName;
	int		m_proxyPort;
	int		m_proxyType;
	BOOL	m_proxyResolve;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionNetworkDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	virtual void sessionFinished(bool success);

	void getProxyInfo(ProxyInfo &info);
	void enableControls();

	SocksSession socksSession;
	HttpSession httpSession;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionNetworkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUseProxy();
	afx_msg void OnProxyTest();
	afx_msg void OnSelchangeProxyType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONNETWORKDLG_H__B2F02B53_1E78_4A26_A4F1_1AAB8AA36D15__INCLUDED_)
