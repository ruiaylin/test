/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2002 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

#if !defined(AFX_GROUPSEARCHRESULTPAGE_H__6C694A22_429E_40D1_9FA6_E736A5E953A9__INCLUDED_)
#define AFX_GROUPSEARCHRESULTPAGE_H__6C694A22_429E_40D1_9FA6_E736A5E953A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupSearchResultPage.h : header file
//

struct GROUP_INFO;

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchResultPage dialog

class CGroupSearchResultPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupSearchResultPage)

// Construction
public:
	CGroupSearchResultPage();
	~CGroupSearchResultPage();

	GROUP_INFO *getResult();

	void onSendError();
	void onGroupListReply(PtrList &l);
	void onSearchGroupReply(GROUP_INFO *info);

// Dialog Data
	//{{AFX_DATA(CGroupSearchResultPage)
	enum { IDD = IDD_GROUP_SEARCH_RESULT };
	CListCtrl	m_resultList;
	CString	m_passwd;
	//}}AFX_DATA

private:
	void deleteAllItems();
	void addResult(GROUP_INFO *info);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupSearchResultPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupSearchResultPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPSEARCHRESULTPAGE_H__6C694A22_429E_40D1_9FA6_E736A5E953A9__INCLUDED_)
