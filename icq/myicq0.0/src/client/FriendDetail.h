#if !defined(AFX_FRIENDDETAIL_H__2825ACF3_540E_11D5_B142_B8DDF6DFE200__INCLUDED_)
#define AFX_FRIENDDETAIL_H__2825ACF3_540E_11D5_B142_B8DDF6DFE200__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FriendDetail.h : header file
//
#include "client.h"
#include "bmpcombox.h"
/////////////////////////////////////////////////////////////////////////////
// CFriendDetailDlg dialog

class CFriendDetailDlg : public CDialog
{
// Construction
public:
	void SetOwnerWnd(CWnd* pwnd){pWnd=pwnd;};
	CFriendDetailDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CFriendDetailDlg)
	enum { IDD = IDD_FRIEND_DETAIL };
	CComboBox	m_ComboSex;
	CBmpComBox	m_ComboFace;
	CString	m_strAddress;
	CString	m_strAge;
	CString	m_strDepartment;
	CString	m_strDescription;
	CString	m_strEmail;
	CString	m_strFax;
	CString	m_strHomepage;
	CString	m_strID;
	CString	m_strName;
	CString	m_strPhone;
	int		m_canbeadd;
	//}}AFX_DATA
	UserInfo* pInfo;
	HICON hIcon;
	CWnd* pWnd;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFriendDetailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(CFriendDetailDlg)
public:
	afx_msg void OnRefresh();
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LRESULT OnShowFriendDetail(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnRecvFriendDetail(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRIENDDETAIL_H__2825ACF3_540E_11D5_B142_B8DDF6DFE200__INCLUDED_)
