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

#if !defined(AFX_GROUPINFODLG_H__07B062D6_B663_4487_BCC3_7B47624ED731__INCLUDED_)
#define AFX_GROUPINFODLG_H__07B062D6_B663_4487_BCC3_7B47624ED731__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupInfoDlg.h : header file
//

#include "GroupBasicPage.h"
#include "GroupMembersPage.h"
#include "GroupOptionPage.h"

class IcqGroup;

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoDlg

class CGroupInfoDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CGroupInfoDlg)

// Construction
public:
	CGroupInfoDlg(IcqGroup *g);

// Attributes
public:
	CGroupBasicPage basicPage;
	CGroupMembersPage membersPage;
	CGroupOptionPage optionPage;

	IcqGroup *group;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupInfoDlg)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGroupInfoDlg();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGroupInfoDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPINFODLG_H__07B062D6_B663_4487_BCC3_7B47624ED731__INCLUDED_)
