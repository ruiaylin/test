#if !defined(AFX_MODIFYPIDLG_H__5F3494D4_5960_11D5_B160_90FDE69ECE00__INCLUDED_)
#define AFX_MODIFYPIDLG_H__5F3494D4_5960_11D5_B160_90FDE69ECE00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyPIDlg.h : header file
//
#include "bmpcombox.h"
/////////////////////////////////////////////////////////////////////////////
// CModifyPIDlg dialog

class CModifyPIDlg : public CDialog
{
// Construction
public:
	CModifyPIDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CModifyPIDlg)
	enum { IDD = IDD_MYSELF_DETAIL };
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
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyPIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyPIDlg)
	afx_msg void OnRefresh();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowChangepwd();
	afx_msg void OnModifypi();
	//}}AFX_MSG
	afx_msg LRESULT OnShowFriendDetail(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnRecvMyDetail(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYPIDLG_H__5F3494D4_5960_11D5_B160_90FDE69ECE00__INCLUDED_)
