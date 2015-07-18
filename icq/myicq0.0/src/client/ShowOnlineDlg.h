#if !defined(AFX_SHOWONLINEDLG_H__3DB7E3FC_5A8E_11D5_B16A_8EF54726C300__INCLUDED_)
#define AFX_SHOWONLINEDLG_H__3DB7E3FC_5A8E_11D5_B16A_8EF54726C300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowOnlineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowOnlineDlg dialog

class CShowOnlineDlg : public CDialog
{
// Construction
public:
	CShowOnlineDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CShowOnlineDlg)
	enum { IDD = IDD_SHOW_ONLINE };
	CButton	m_nextbutton;
	CButton	m_lastbutton;
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowOnlineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	HICON hIcon;
	CFriendDetailDlg * m_pDlg;	
	UserInfo Info;	
	BOOL bCancel;
	BOOL bClose;
	BOOL bStartSearch;
	CMsg3 msg;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShowOnlineDlg)
	afx_msg void OnNextpage();
	afx_msg void OnLastpage();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRefresh();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddFriend();
	afx_msg void OnFriendDetail();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnRecvShowOnline(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWONLINEDLG_H__3DB7E3FC_5A8E_11D5_B16A_8EF54726C300__INCLUDED_)
