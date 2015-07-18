#if !defined(AFX_LOOKDLG_H__E1A23954_51A9_11D5_B139_E09F3742E700__INCLUDED_)
#define AFX_LOOKDLG_H__E1A23954_51A9_11D5_B139_E09F3742E700__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LookDlg.h : header file
//
#include "bmpbutton.h"

/////////////////////////////////////////////////////////////////////////////
// CLookDlg dialog

class CLookDlg : public CDialog
{
// Construction
public:
	CLookDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CLookDlg)
	enum { IDD = IDD_LOOK_MESS };
	CBmpButton	m_headBmp;
	CEdit	m_TalkRecord;
	CString	m_strTime;
	CString	m_strTalk;
	CString	m_strID;
	CString	m_strName;
	CString	m_strEmail;
	CString	m_strDate;
	//}}AFX_DATA
	UserInfo *pInfo;
	FriendState *pState;
	int* piCurMsg;
	BOOL m_bClose;
	
	HICON hIcon;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLookDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLookDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowTalk();
	afx_msg void OnNext();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnShowDetail();
	//}}AFX_MSG
	afx_msg LRESULT OnRecvMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnRefreshData(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOOKDLG_H__E1A23954_51A9_11D5_B139_E09F3742E700__INCLUDED_)
