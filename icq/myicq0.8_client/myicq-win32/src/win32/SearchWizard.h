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

#if !defined(AFX_SEARCHWIZARD_H__2BE6BCA0_F7FA_4E14_9D22_630FA15B627B__INCLUDED_)
#define AFX_SEARCHWIZARD_H__2BE6BCA0_F7FA_4E14_9D22_630FA15B627B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchWizard.h : header file
//

#include "icqwindow.h"
#include "SearchModeDlg.h"
#include "SearchCustomDlg.h"
#include "SearchResultDlg.h"
#include "SearchAddDlg.h"

enum {
	SEARCH_MODE_ONLINE,
	SEARCH_MODE_CUSTOM
};


/////////////////////////////////////////////////////////////////////////////
// CSearchWizard

class CSearchWizard : public CPropertySheet, public IcqWindow
{
	DECLARE_DYNAMIC(CSearchWizard)

// Construction
public:
	CSearchWizard();

	void onAck(uint32 seq) {
		if (GetActivePage() == &addDlg)
			addDlg.onAck(seq);
	}
	void onUserFound(PtrList *l) {
		if (GetActivePage() == &resultDlg)
			resultDlg.onUserFound(l);
	}
	void onAddFriendReply(uint8 result) {
		if (GetActivePage() == &addDlg)
			addDlg.onAddFriendReply(result);
	}

// Attributes
public:
	CSearchModeDlg modeDlg;
	CSearchCustomDlg customDlg;
	CSearchResultDlg resultDlg;
	CSearchAddDlg addDlg;

// Operations
public:

// Overrides
public:
	BOOL Create(CWnd *pParentWnd = NULL);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchWizard)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSearchWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchWizard)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHWIZARD_H__2BE6BCA0_F7FA_4E14_9D22_630FA15B627B__INCLUDED_)
