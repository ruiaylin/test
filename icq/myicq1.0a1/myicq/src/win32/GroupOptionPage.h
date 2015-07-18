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

#if !defined(AFX_GROUPOPTIONPAGE_H__FFEA41D4_8ED4_4359_A93C_807073DCF343__INCLUDED_)
#define AFX_GROUPOPTIONPAGE_H__FFEA41D4_8ED4_4359_A93C_807073DCF343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupOptionPage.h : header file
//

class IcqGroup;

/////////////////////////////////////////////////////////////////////////////
// CGroupOptionPage dialog

class CGroupOptionPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupOptionPage)

// Construction
public:
	CGroupOptionPage();
	~CGroupOptionPage();

	IcqGroup *getGroup();

// Dialog Data
	//{{AFX_DATA(CGroupOptionPage)
	enum { IDD = IDD_GROUP_INFO_OPTION };
	int		m_msgOption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupOptionPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupOptionPage)
	afx_msg void OnModify();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPOPTIONPAGE_H__FFEA41D4_8ED4_4359_A93C_807073DCF343__INCLUDED_)
