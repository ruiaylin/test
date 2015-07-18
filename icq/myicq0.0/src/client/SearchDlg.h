#if !defined(AFX_SEARCHDLG_H__3DB7E3F6_5A8E_11D5_B16A_8EF54726C300__INCLUDED_)
#define AFX_SEARCHDLG_H__3DB7E3F6_5A8E_11D5_B16A_8EF54726C300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchDlg.h : header file
//
class CFriendDetailDlg;
/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

class CSearchDlg : public CDialog
{
// Construction
public:
	CSearchDlg(CWnd* pParent = NULL);   // standard constructor
	CClientApp* GetApp(){return (CClientApp*)AfxGetApp();};
// Dialog Data
	//{{AFX_DATA(CSearchDlg)
	enum { IDD = IDD_SEARCH };
	CEdit	m_Name;
	CEdit	m_ID;
	CListCtrl	m_List;
	int		m_RID;
	//}}AFX_DATA

	HICON hIcon;
	CFriendDetailDlg * m_pDlg;	
	UserInfo Info;
	
	BOOL bCancel;
	BOOL bStartSearch;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSearchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioId();
	afx_msg void OnRadioName();
	afx_msg void OnFriendsDetail();
	afx_msg void OnAddFriend();
	virtual void OnOK();
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnRecvSearch(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHDLG_H__3DB7E3F6_5A8E_11D5_B16A_8EF54726C300__INCLUDED_)
