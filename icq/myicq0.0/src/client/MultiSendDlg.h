#if !defined(AFX_MULTISENDDLG_H__5F3494D3_5960_11D5_B160_90FDE69ECE00__INCLUDED_)
#define AFX_MULTISENDDLG_H__5F3494D3_5960_11D5_B160_90FDE69ECE00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiSendDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMultiSendDlg dialog

class CMultiSendDlg : public CDialog
{
// Construction
public:
	CMultiSendDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CMultiSendDlg)
	enum { IDD = IDD_MULTI_SEND };
	CEdit	m_EditMsg;
	CEdit	m_EditName;
	CEdit	m_EditId;
	int		m_radio;
	//}}AFX_DATA
	HICON hIcon;
	BOOL m_bCancel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiSendDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMultiSendDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTISENDDLG_H__5F3494D3_5960_11D5_B160_90FDE69ECE00__INCLUDED_)
