#if !defined(AFX_ADDFRIENDDLG_H__BFD829F6_64CB_11D5_B1AD_821E18DB138B__INCLUDED_)
#define AFX_ADDFRIENDDLG_H__BFD829F6_64CB_11D5_B1AD_821E18DB138B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddFriendDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddFriendDlg dialog
#include "bmpbutton.h"

class CAddFriendDlg : public CDialog
{
// Construction
public:
	CAddFriendDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
	DWORD uID;
// Dialog Data
	//{{AFX_DATA(CAddFriendDlg)
	enum { IDD = IDD_ADD_FRIEND };
	CBmpButton	m_headbmp;
	CStatic	m_Rect;
	CString	m_strID;
	CString	m_strName;
	//}}AFX_DATA

	UserInfo Info;
	CFriendDetailDlg * pDetailDlg;
	CWnd *pWnd;
	BOOL bStartAdd;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFriendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddFriendDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFriendDetail();
	//}}AFX_MSG
	afx_msg LRESULT OnRecvAddFriendRes(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnNoticeAddFriend(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFRIENDDLG_H__BFD829F6_64CB_11D5_B1AD_821E18DB138B__INCLUDED_)
