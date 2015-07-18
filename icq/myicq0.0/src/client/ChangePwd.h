#if !defined(AFX_CHANGEPWD_H__3DB7E3FB_5A8E_11D5_B16A_8EF54726C300__INCLUDED_)
#define AFX_CHANGEPWD_H__3DB7E3FB_5A8E_11D5_B16A_8EF54726C300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangePwd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangePwd dialog

class CChangePwd : public CDialog
{
// Construction
public:
	CChangePwd(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CChangePwd)
	enum { IDD = ID_CHANGE_PWD };
	CEdit	m_oldpwdEdit;
	CEdit	m_newpwd2Edit;
	CEdit	m_newpwdEdit;
	BOOL	m_bCanModify;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangePwd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangePwd)
	afx_msg void OnStartChang();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEPWD_H__3DB7E3FB_5A8E_11D5_B16A_8EF54726C300__INCLUDED_)
