#if !defined(AFX_TALKDLG_H__E1A23953_51A9_11D5_B139_E09F3742E700__INCLUDED_)
#define AFX_TALKDLG_H__E1A23953_51A9_11D5_B139_E09F3742E700__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TalkDlg.h : header file
//
#include "bmpbutton.h"

/////////////////////////////////////////////////////////////////////////////
// CTalkDlg dialog


class CTalkDlg : public CDialog
{
// Construction
public:
	CTalkDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};	

// Dialog Data
	//{{AFX_DATA(CTalkDlg)
	enum { IDD = IDD_SEND_MESS };
	CBmpButton	m_headBmp;
	CEdit	m_TalkRecord;
	CEdit	m_TalkEdit;
	CString	m_strEmail;
	CString	m_strID;
	CString	m_strName;
	//}}AFX_DATA

	UserInfo * pInfo;
	FriendState * pState;	
	int nPhotoId;
	HICON hIcon;

	BOOL bClosed;
	BOOL m_bCancel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTalkDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTalkDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnShowTalk();
	afx_msg void OnShowDetail();	
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnSendMsg(WPARAM wParam,LPARAM lParam);	
	afx_msg LRESULT OnRefreshData(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALKDLG_H__E1A23953_51A9_11D5_B139_E09F3742E700__INCLUDED_)
