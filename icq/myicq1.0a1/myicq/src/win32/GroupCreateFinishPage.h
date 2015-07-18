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

#if !defined(AFX_GROUPCREATEFINISHPAGE_H__A2B89947_0EF0_4B7C_BD07_46ECD13DEEBA__INCLUDED_)
#define AFX_GROUPCREATEFINISHPAGE_H__A2B89947_0EF0_4B7C_BD07_46ECD13DEEBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupCreateFinishPage.h : header file
//

#include "AnimFace.h"

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateFinishPage dialog

class CGroupCreateFinishPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupCreateFinishPage)

// Construction
public:
	CGroupCreateFinishPage();
	~CGroupCreateFinishPage();

	void onSendError();
	void onCreateGroupReply(uint32 id);

// Dialog Data
	//{{AFX_DATA(CGroupCreateFinishPage)
	enum { IDD = IDD_GROUP_CREATE_FINISH };
	CAnimFace	m_faceLabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupCreateFinishPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupCreateFinishPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPCREATEFINISHPAGE_H__A2B89947_0EF0_4B7C_BD07_46ECD13DEEBA__INCLUDED_)
