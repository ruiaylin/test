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

#if !defined(AFX_GROUPSEARCHWIZARD_H__0A045BD2_3883_4549_8F2C_7233C570CEE4__INCLUDED_)
#define AFX_GROUPSEARCHWIZARD_H__0A045BD2_3883_4549_8F2C_7233C570CEE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupSearchWizard.h : header file
//

#include "icqwindow.h"
#include "GroupSearchModePage.h"
#include "GroupSearchTypePage.h"
#include "GroupSearchResultPage.h"
#include "GroupSearchFinishPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchWizard

class CGroupSearchWizard : public CPropertySheet, public IcqWindow
{
	DECLARE_DYNAMIC(CGroupSearchWizard)

// Construction
public:
	CGroupSearchWizard();

	BOOL Create(CWnd *parent = NULL);

	virtual void onSendError(uint32 seq) {
		if (GetActivePage() == &resultPage)
			resultPage.onSendError();
		else if (GetActivePage() == &finishPage)
			finishPage.onSendError();
	}

	void onGroupListReply(PtrList &l) {
		if (GetActivePage() == &resultPage)
			resultPage.onGroupListReply(l);
	}
	void onEnterGroupReply(uint32 id, uint16 error) {
		if (GetActivePage() == &finishPage)
			finishPage.onEnterGroupReply(id, error);
	}
	void onSearchGroupReply(GROUP_INFO *info) {
		if (GetActivePage() == &resultPage)
			resultPage.onSearchGroupReply(info);
	}

// Attributes
public:
	CGroupSearchModePage modePage;
	CGroupSearchTypePage typePage;
	CGroupSearchResultPage resultPage;
	CGroupSearchFinishPage finishPage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupSearchWizard)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGroupSearchWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGroupSearchWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPSEARCHWIZARD_H__0A045BD2_3883_4549_8F2C_7233C570CEE4__INCLUDED_)
