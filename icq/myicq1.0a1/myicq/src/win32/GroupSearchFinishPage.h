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

#if !defined(AFX_GROUPSEARCHFINISHPAGE_H__FD4F4F0D_8BC3_422B_8CB6_106E7E9AA292__INCLUDED_)
#define AFX_GROUPSEARCHFINISHPAGE_H__FD4F4F0D_8BC3_422B_8CB6_106E7E9AA292__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupSearchFinishPage.h : header file
//

#include "AnimFace.h"

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchFinishPage dialog

class CGroupSearchFinishPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupSearchFinishPage)

// Construction
public:
	CGroupSearchFinishPage();
	~CGroupSearchFinishPage();

	void onSendError();
	void onEnterGroupReply(uint32 id, uint16 error);

// Dialog Data
	//{{AFX_DATA(CGroupSearchFinishPage)
	enum { IDD = IDD_GROUP_SEARCH_FINISH };
	CAnimFace	m_faceLabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupSearchFinishPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupSearchFinishPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPSEARCHFINISHPAGE_H__FD4F4F0D_8BC3_422B_8CB6_106E7E9AA292__INCLUDED_)
