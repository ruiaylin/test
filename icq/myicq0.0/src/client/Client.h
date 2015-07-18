// Client.h : main header file for the CLIENT application
//

#if !defined(AFX_CLIENT_H__EDC351A4_4F64_11D5_B12C_CA5F4F71E900__INCLUDED_)
#define AFX_CLIENT_H__EDC351A4_4F64_11D5_B12C_CA5F4F71E900__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Msg.h"
#include "ClientSocket.h"

/////////////////////////////////////////////////////////////////////////////
// CClientApp:
// See Client.cpp for the implementation of this class
//

class CClientApp : public CWinApp
{
public:
	CClientApp();
	BOOL m_bOnlineState;
	int m_nNumberHeadBmp;
	CArray<UserInfo*,UserInfo*>m_aUserInfo;
	CArray<UserInfo*,UserInfo*>m_aStranger;
	CArray<FriendState*,FriendState*>m_aUserState;
	CArray<FriendState*,FriendState*>m_aStrangerState;
	CArray<SaveMsg*,SaveMsg*>m_aSysMsg;
	int m_nCurSysMsg;
	CArray<SaveMsg*,SaveMsg*>m_aNoticeMsg;
	int m_nCurNotice;
	CArray<SaveMsg*,SaveMsg*>m_aOtherMsg;
	int m_nCurOther;	
	CClientSocket m_Socket;
	DWORD m_uCurrentUserID;
	CString m_strCurIDpwd;
	CString m_strCurUserDir;
	CString m_strServerIP;
	DWORD m_uServerIP;
	DWORD m_uServerPort;
	CImageList m_imaLarge,m_imaSmall,m_imaHead,m_imaSmallHead;
	CBitmap** m_paBitmap;
	CBitmap** m_paSmallBitmap;	
	int m_nBitmapNum;
	int m_nTotalOnline;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	void SaveCurIDToFile();
	BOOL SaveUserInfo();
	BOOL LoadFriendInfo();
	BOOL LoadFaceBmp();
	BOOL LoadLocalAllUserID(CArray<int,int>&aID);
	//{{AFX_MSG(CClientApp)
	afx_msg void OnAppAbout();
	afx_msg void OnSysSetup();
	afx_msg void OnSendFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENT_H__EDC351A4_4F64_11D5_B12C_CA5F4F71E900__INCLUDED_)
