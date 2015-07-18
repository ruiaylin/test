#if !defined(AFX_SHOWBROADCASTDLG_H__B80F3FB3_5745_11D5_B154_DFCA0DE9CC00__INCLUDED_)
#define AFX_SHOWBROADCASTDLG_H__B80F3FB3_5745_11D5_B154_DFCA0DE9CC00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowBroadcastDlg.h : header file
//
#include "bmpbutton.h"
/////////////////////////////////////////////////////////////////////////////
// CShowBroadcastDlg dialog

class CShowBroadcastDlg : public CDialog
{
// Construction
public:
	CShowBroadcastDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowBroadcastDlg)
	enum { IDD = IDD_SHOWBROADCAST };
	CBmpButton	m_headbmp;
	CEdit	m_EditMsg;
	CEdit	m_EditRecord;
	CString	m_strDate;
	CString	m_strEmail;
	CString	m_strID;
	CString	m_strName;
	CString	m_strTime;
	//}}AFX_DATA

	HICON hIcon;
	UserInfo Info,*pInfo;
	CFriendDetailDlg * pDetailDlg;
	BOOL m_bClose;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowBroadcastDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};

	// Generated message map functions
	//{{AFX_MSG(CShowBroadcastDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNext();
	afx_msg void OnShowHistory();
	afx_msg void OnShowDetail();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	LRESULT OnShowNotice(WPARAM wParam,LPARAM lParam);
	LRESULT OnRefreshData(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWBROADCASTDLG_H__B80F3FB3_5745_11D5_B154_DFCA0DE9CC00__INCLUDED_)
