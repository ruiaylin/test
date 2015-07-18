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

#if !defined(AFX_GROUPBASICPAGE_H__B5269CE2_7C07_4566_B456_9D46DF904C11__INCLUDED_)
#define AFX_GROUPBASICPAGE_H__B5269CE2_7C07_4566_B456_9D46DF904C11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupBasicPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupBasicPage dialog

class CGroupBasicPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupBasicPage)

// Construction
public:
	CGroupBasicPage();
	~CGroupBasicPage();

// Dialog Data
	//{{AFX_DATA(CGroupBasicPage)
	enum { IDD = IDD_GROUP_INFO_BASIC };
	UINT	m_groupID;
	CString	m_groupName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupBasicPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupBasicPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPBASICPAGE_H__B5269CE2_7C07_4566_B456_9D46DF904C11__INCLUDED_)
