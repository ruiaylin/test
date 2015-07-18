// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__EDC351A8_4F64_11D5_B12C_CA5F4F71E900__INCLUDED_)
#define AFX_MAINFRM_H__EDC351A8_4F64_11D5_B12C_CA5F4F71E900__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "trayicon.h"
#include "GfxOutBarCtrl.h"
#include "GfxPopupMenu.h"

class CSendToAllDlg;
class CMultiSendDlg;
class CSearchDlg;
class CModifyPIDlg;
class CShowOnlineDlg;
class CSendBroadcastDlg;
class CShowBroadcastDlg;
class CShowAddMsgDlg;
class CAddFriendDlg;
#define WM_MY_TRAY_NOTIFICATION  WM_USER+100

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

	CGfxPopupMenu menu1,menu2;
public:

	CGfxOutBarCtrl  m_wndBar;	
	CTrayIcon       m_TrayIcon;

	int nCurSelID;
	int nGroupFlag; //1是陌生人,2是黑名单，0或大于2的数是好友

	CSearchDlg *m_pSearch;
	CModifyPIDlg *m_pModifyPI;
	CMultiSendDlg *m_pMultiSend;
	CSendToAllDlg *m_pSendToAll;
	CShowOnlineDlg * m_pShowOnline;
	CSendBroadcastDlg * m_pSendBroad;
	CShowBroadcastDlg * m_pShowNotice;
	CShowAddMsgDlg * m_pShowOther;
	CAddFriendDlg * m_pAddFriendDlg;
	CArray<UserInfo*,UserInfo*>m_aTempInfo;	

	BOOL m_bRecvTotal;
	int nCurSelIndex;
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSendMessage();
	afx_msg void OnUpdateSendMessage(CCmdUI* pCmdUI);
	afx_msg void OnSendEmail();
	afx_msg void OnUpdateSendEmail(CCmdUI* pCmdUI);
	afx_msg void OnFriendDetail();
	afx_msg void OnPersonHomepage();
	afx_msg void OnUpdatePersonHomepage(CCmdUI* pCmdUI);
	afx_msg void OnOffline();
	afx_msg void OnOnhide();
	afx_msg void OnOnline();
	afx_msg void OnSearch();
	afx_msg void OnPersoninfoSetup();
	afx_msg void OnMultisend();
	afx_msg void OnSendtoAll();
	afx_msg void OnMessageManage();
	afx_msg void OnShowOnline();
	afx_msg void OnSendBroadcast();
	afx_msg void OnAddFriend();
	//}}AFX_MSG
	afx_msg LRESULT OnTrayNotification(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnOutbarNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ProcRecv(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnAddFriendRefresh(WPARAM wParam,LPARAM lParam);	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__EDC351A8_4F64_11D5_B12C_CA5F4F71E900__INCLUDED_)
