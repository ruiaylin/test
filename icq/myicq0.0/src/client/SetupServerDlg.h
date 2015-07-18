#if !defined(AFX_SETUPSERVERDLG_H__7E1E6136_50F5_11D5_B135_E9B14F7CE700__INCLUDED_)
#define AFX_SETUPSERVERDLG_H__7E1E6136_50F5_11D5_B135_E9B14F7CE700__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupServerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupServerDlg dialog

class CSetupServerDlg : public CDialog
{
// Construction
public:
	CSetupServerDlg(CWnd* pParent = NULL);   // standard constructor
	DWORD uServerIP;
	DWORD uPort;
// Dialog Data
	//{{AFX_DATA(CSetupServerDlg)
	enum { IDD = IDD_SEVER_SETUP };
	CString	m_strServerIP;
	UINT	m_port;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupServerDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPSERVERDLG_H__7E1E6136_50F5_11D5_B135_E9B14F7CE700__INCLUDED_)
