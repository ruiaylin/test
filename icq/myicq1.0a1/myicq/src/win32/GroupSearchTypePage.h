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

#if !defined(AFX_GROUPSEARCHTYPEPAGE_H__3412C6B9_60D9_4F5E_8F65_B38792310F31__INCLUDED_)
#define AFX_GROUPSEARCHTYPEPAGE_H__3412C6B9_60D9_4F5E_8F65_B38792310F31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupSearchTypePage.h : header file
//

struct GROUP_TYPE_INFO;

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchTypePage dialog

class CGroupSearchTypePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupSearchTypePage)

// Construction
public:
	CGroupSearchTypePage();
	~CGroupSearchTypePage();

	int getGroupType();

// Dialog Data
	//{{AFX_DATA(CGroupSearchTypePage)
	enum { IDD = IDD_GROUP_SEARCH_TYPE };
	CListCtrl	m_typeList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupSearchTypePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupSearchTypePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPSEARCHTYPEPAGE_H__3412C6B9_60D9_4F5E_8F65_B38792310F31__INCLUDED_)
