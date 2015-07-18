#if !defined(AFX_SENDBROADCASTDLG_H__5F3494D6_5960_11D5_B160_90FDE69ECE00__INCLUDED_)
#define AFX_SENDBROADCASTDLG_H__5F3494D6_5960_11D5_B160_90FDE69ECE00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendBroadcastDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendBroadcastDlg dialog

class CSendBroadcastDlg : public CDialog
{
// Construction
public:
	CSendBroadcastDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CSendBroadcastDlg)
	enum { IDD = IDD_SEND_BROADCAST };
	CEdit	m_EditPwd;
	CEdit	m_EditMsg;
	//}}AFX_DATA
	HICON hIcon;
	BOOL m_bCancel;	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendBroadcastDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendBroadcastDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDBROADCASTDLG_H__5F3494D6_5960_11D5_B160_90FDE69ECE00__INCLUDED_)
