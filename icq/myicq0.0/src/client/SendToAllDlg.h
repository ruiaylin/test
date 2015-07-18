#if !defined(AFX_SENDTOALLDLG_H__5F3494D5_5960_11D5_B160_90FDE69ECE00__INCLUDED_)
#define AFX_SENDTOALLDLG_H__5F3494D5_5960_11D5_B160_90FDE69ECE00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendToAllDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendToAllDlg dialog

class CSendToAllDlg : public CDialog
{
// Construction
public:
	CSendToAllDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CSendToAllDlg)
	enum { IDD = IDD_SENDTO_ALL };
	CEdit	m_Edit;
	//}}AFX_DATA
	HICON hIcon;
	BOOL m_bCancel;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendToAllDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendToAllDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDTOALLDLG_H__5F3494D5_5960_11D5_B160_90FDE69ECE00__INCLUDED_)
