#if !defined(AFX_SHOWADDMSGDLG_H__B80F3FB4_5745_11D5_B154_DFCA0DE9CC00__INCLUDED_)
#define AFX_SHOWADDMSGDLG_H__B80F3FB4_5745_11D5_B154_DFCA0DE9CC00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowAddMsgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowAddMsgDlg dialog

class CShowAddMsgDlg : public CDialog
{
// Construction
public:
	CShowAddMsgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowAddMsgDlg)
	enum { IDD = IDD_SHOW_ADD_FRIEND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowAddMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShowAddMsgDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWADDMSGDLG_H__B80F3FB4_5745_11D5_B154_DFCA0DE9CC00__INCLUDED_)
