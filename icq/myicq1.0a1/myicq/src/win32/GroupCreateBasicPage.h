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

#if !defined(AFX_GROUPCREATEBASICPAGE_H__8A3264B9_3026_4712_A5DF_5ABEA5060ACC__INCLUDED_)
#define AFX_GROUPCREATEBASICPAGE_H__8A3264B9_3026_4712_A5DF_5ABEA5060ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupCreateBasicPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateBasicPage dialog

class CGroupCreateBasicPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupCreateBasicPage)

// Construction
public:
	CGroupCreateBasicPage();
	~CGroupCreateBasicPage();

// Dialog Data
	//{{AFX_DATA(CGroupCreateBasicPage)
	enum { IDD = IDD_GROUP_CREATE_BASIC };
	CString	m_groupName;
	CString	m_passwd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupCreateBasicPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupCreateBasicPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPCREATEBASICPAGE_H__8A3264B9_3026_4712_A5DF_5ABEA5060ACC__INCLUDED_)
