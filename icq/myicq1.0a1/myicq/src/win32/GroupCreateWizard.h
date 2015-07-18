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

#if !defined(AFX_GROUPCREATEWIZARD_H__04029553_C0A5_485C_AFC2_E50F740434B4__INCLUDED_)
#define AFX_GROUPCREATEWIZARD_H__04029553_C0A5_485C_AFC2_E50F740434B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupCreateWizard.h : header file
//

#include "icqwindow.h"
#include "GroupCreateTypePage.h"
#include "GroupCreateBasicPage.h"
#include "GroupCreateFinishPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateWizard

class CGroupCreateWizard : public CPropertySheet, public IcqWindow
{
	DECLARE_DYNAMIC(CGroupCreateWizard)

// Construction
public:
	CGroupCreateWizard();

// Attributes
public:
	CGroupCreateTypePage typePage;
	CGroupCreateBasicPage basicPage;
	CGroupCreateFinishPage finishPage;

	int groupType;
	CString groupPlugin;

// Operations
public:
	BOOL Create(CWnd *parent = NULL);

	virtual void onSendError(uint32 seq) {
		if (GetActivePage() == &finishPage)
			finishPage.onSendError();
	}
	void onCreateGroupReply(uint32 id) {
		if (GetActivePage() == &finishPage)
			finishPage.onCreateGroupReply(id);
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupCreateWizard)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGroupCreateWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGroupCreateWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPCREATEWIZARD_H__04029553_C0A5_485C_AFC2_E50F740434B4__INCLUDED_)
