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

#if !defined(AFX_OUTBAREDIT_H__18A5D73E_2167_4872_9C44_61123C233826__INCLUDED_)
#define AFX_OUTBAREDIT_H__18A5D73E_2167_4872_9C44_61123C233826__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutBarEdit.h : header file
//

class OutBarCtrl;

/////////////////////////////////////////////////////////////////////////////
// OutBarEdit window

class OutBarEdit : public CEdit
{
// Construction
public:
	OutBarEdit(OutBarCtrl *l, int obj, int index);

// Attributes
public:
	int obj;
	int index;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OutBarEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~OutBarEdit();

private:
	OutBarCtrl *listener;

	// Generated message map functions
protected:
	//{{AFX_MSG(OutBarEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTBAREDIT_H__18A5D73E_2167_4872_9C44_61123C233826__INCLUDED_)
