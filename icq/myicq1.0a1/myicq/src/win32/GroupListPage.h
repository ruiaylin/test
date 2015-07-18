#if !defined(AFX_GROUPLISTPAGE_H__80779B49_90B7_4535_AD36_854EEB0D7A63__INCLUDED_)
#define AFX_GROUPLISTPAGE_H__80779B49_90B7_4535_AD36_854EEB0D7A63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupListPage.h : header file
//

struct GROUP_INFO;

/////////////////////////////////////////////////////////////////////////////
// CGroupListPage dialog

class CGroupListPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupListPage)

// Construction
public:
	CGroupListPage();
	~CGroupListPage();

	GROUP_INFO *getGroupInfo();

	void onGroupListReply(GROUP_INFO info[], int n);

// Dialog Data
	//{{AFX_DATA(CGroupListPage)
	enum { IDD = IDD_GROUP_LIST };
	CListCtrl	m_groupList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupListPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupListPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPLISTPAGE_H__80779B49_90B7_4535_AD36_854EEB0D7A63__INCLUDED_)
