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

#if !defined(AFX_GROUPCREATETYPEPAGE_H__8CF4BE44_53D5_48C2_B568_66B4BB066C1C__INCLUDED_)
#define AFX_GROUPCREATETYPEPAGE_H__8CF4BE44_53D5_48C2_B568_66B4BB066C1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupCreateTypePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupCreateTypePage dialog

class CGroupCreateTypePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupCreateTypePage)

// Construction
public:
	CGroupCreateTypePage();
	~CGroupCreateTypePage();

// Dialog Data
	//{{AFX_DATA(CGroupCreateTypePage)
	enum { IDD = IDD_GROUP_CREATE_TYPE };
	CListCtrl	m_typeList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupCreateTypePage)
	public:
	virtual BOOL OnSetActive();
	virtual void OnCancel();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupCreateTypePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPCREATETYPEPAGE_H__8CF4BE44_53D5_48C2_B568_66B4BB066C1C__INCLUDED_)
