// Server.h : main header file for the SERVER application
//

#if !defined(AFX_SERVER_H__C3189977_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_)
#define AFX_SERVER_H__C3189977_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "common.h"
#include "socket.h"

/////////////////////////////////////////////////////////////////////////////
// CServerApp:
// See Server.cpp for the implementation of this class
//

class CServerApp : public CWinApp
{
public:
	void SaveMyProfileSettings();
	BOOL LoadMyProfileSettings();
	CServerApp();
	CServerSocket m_Socket;
	CArray<DWORD,DWORD>m_anPort;
	int m_nSendNum;  // the number of send data socket
	int m_nTimeOut;
	BOOL m_bServerStarted;
	UserOnline* m_pUsers;
	CDatabase m_Database;
	CString m_sConnectString;
	int m_nNumberOnline;
	int m_nMaxUserId;
	int m_nTotalUserNumber;
	int m_nRecvMsg;
	CString m_strBroadcastPwd;
	CString m_strDataSource;
	CString m_strDataSUID;
	CString m_strDataSPwd;
	CString m_strLocalIP;
	CCriticalSection m_criticalMaxUserId;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CServerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnSetupServer();
	afx_msg void OnStartServer();
	afx_msg void OnUpdateStartServer(CCmdUI* pCmdUI);
	afx_msg void OnUserOnline();
	afx_msg void OnCloseServer();
	afx_msg void OnUpdateCloseServer(CCmdUI* pCmdUI);
	afx_msg void OnAppExit();
	afx_msg void OnUpdateUserOnline(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVER_H__C3189977_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_)
