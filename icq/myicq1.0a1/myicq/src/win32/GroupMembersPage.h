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

#if !defined(AFX_GROUPMEMBERSPAGE_H__F0608FB8_4955_401C_891B_DD9FD07F5028__INCLUDED_)
#define AFX_GROUPMEMBERSPAGE_H__F0608FB8_4955_401C_891B_DD9FD07F5028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupMembersPage.h : header file
//

class GroupMember;

/////////////////////////////////////////////////////////////////////////////
// CGroupMembersPage dialog

class CGroupMembersPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupMembersPage)

// Construction
public:
	CGroupMembersPage();
	~CGroupMembersPage();

// Dialog Data
	//{{AFX_DATA(CGroupMembersPage)
	enum { IDD = IDD_GROUP_INFO_MEMBERS };
	CListCtrl	m_memberList;
	//}}AFX_DATA

private:
	void addMember(GroupMember *m);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupMembersPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupMembersPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPMEMBERSPAGE_H__F0608FB8_4955_401C_891B_DD9FD07F5028__INCLUDED_)
