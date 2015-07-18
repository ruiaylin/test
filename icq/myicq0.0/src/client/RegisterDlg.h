#if !defined(AFX_REGISTERDLG_H__22830FE1_511D_11D5_B136_CFA2365E9C00__INCLUDED_)
#define AFX_REGISTERDLG_H__22830FE1_511D_11D5_B136_CFA2365E9C00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterDlg.h : header file
//
#include "bmpcombox.h"

class CClientApp; 
/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog

class CRegisterDlg : public CDialog
{
// Construction
public:
	CRegisterDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CRegisterDlg)
	enum { IDD = IDD_REGISTER };
	CBmpComBox	m_headbmp;
	CEdit	m_userpwd;
	CEdit	m_userid;
	CComboBox	m_sex;
	CEdit	m_pwd1;
	CEdit	m_pwd;
	CEdit	m_phone;
	CEdit	m_name;
	CEdit	m_homepage;
	CEdit	m_fax;
	CEdit	m_email;
	CEdit	m_description;
	CEdit	m_department;
	CButton	m_applyid;
	CButton	m_allowall;
	CButton m_needcheck;
	CButton m_notallow;
	CEdit	m_age;
	CEdit	m_address;
	CString	m_strServerIP;
	UINT	m_uServerPort;
	//}}AFX_DATA
	DWORD m_uServerIP;
	DWORD m_uUserID;	
	CString m_strPassword;
	BOOL m_bApplyID;
	CMsgPerson msg;
	HICON hIcon;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUseHaveid();
	afx_msg void OnApplyId();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnRecvMsg(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERDLG_H__22830FE1_511D_11D5_B136_CFA2365E9C00__INCLUDED_)
