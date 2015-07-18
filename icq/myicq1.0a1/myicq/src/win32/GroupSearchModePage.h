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

#if !defined(AFX_GROUPSEARCHMODEPAGE_H__8E76CD2A_C731_48CA_88DF_81221973891D__INCLUDED_)
#define AFX_GROUPSEARCHMODEPAGE_H__8E76CD2A_C731_48CA_88DF_81221973891D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupSearchModePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchModePage dialog

class CGroupSearchModePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupSearchModePage)

// Construction
public:
	CGroupSearchModePage();
	~CGroupSearchModePage();

// Dialog Data
	//{{AFX_DATA(CGroupSearchModePage)
	enum { IDD = IDD_GROUP_SEARCH_MODE };
	int		m_mode;
	UINT	m_groupID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupSearchModePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupSearchModePage)
	afx_msg void OnModeID();
	afx_msg void OnModeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPSEARCHMODEPAGE_H__8E76CD2A_C731_48CA_88DF_81221973891D__INCLUDED_)
