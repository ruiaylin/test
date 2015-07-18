#if !defined(AFX_SETUPDLG_H__F1C8F02F_532D_11D5_B13F_86ACD560F400__INCLUDED_)
#define AFX_SETUPDLG_H__F1C8F02F_532D_11D5_B13F_86ACD560F400__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog

class CSetupDlg : public CDialog
{
// Construction
public:
	CSetupDlg(CWnd* pParent = NULL);   // standard constructor
	CServerApp* GetApp(){(CServerApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CSetupDlg)
	enum { IDD = IDD_SETUP };
	CListBox	m_ListPost;
	CString	m_strIP;
	CString	m_strPort;
	CString	m_strPwd;
	UINT	m_uSendNum;
	CString	m_strUid;
	CString	m_strDS;
	CString	m_strBroadcastPwd;
	//}}AFX_DATA

	CArray<CString,CString>m_aStr;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupDlg)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDefaultSetup();
	afx_msg void OnConfigDs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDLG_H__F1C8F02F_532D_11D5_B13F_86ACD560F400__INCLUDED_)
