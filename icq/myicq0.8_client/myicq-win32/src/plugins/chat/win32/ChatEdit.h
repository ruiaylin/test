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

#if !defined(AFX_CHATEDIT_H__701CB929_5890_4F8A_959B_7E169DEA7C83__INCLUDED_)
#define AFX_CHATEDIT_H__701CB929_5890_4F8A_959B_7E169DEA7C83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChatEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChatEdit window

class CChatEdit : public CEdit
{
// Construction
public:
	CChatEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChatEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChatEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHATEDIT_H__701CB929_5890_4F8A_959B_7E169DEA7C83__INCLUDED_)
