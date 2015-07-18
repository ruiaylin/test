#if !defined(AFX_LOGINDLG_H__91D83026_50C7_11D5_B134_90413C9CFD00__INCLUDED_)
#define AFX_LOGINDLG_H__91D83026_50C7_11D5_B134_90413C9CFD00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CDialog
{
// Construction
public:
	void SetUserID(CArray<int,int>&aID);
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoginDlg)
	enum { IDD = IDD_LOGIN };
	CEdit	m_EditPwd;
	CComboBox	m_IdList;
	BOOL	m_LoginType;
	HICON m_hIcon;
	//}}AFX_DATA
	CString pwd;
	DWORD uServerIP;
	DWORD uPort;
	DWORD uCurID;
	int lresult;
	CArray<CString,CString>m_aID;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginDlg)
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUserRegistr();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	afx_msg LRESULT ProcRecv(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINDLG_H__91D83026_50C7_11D5_B134_90413C9CFD00__INCLUDED_)
