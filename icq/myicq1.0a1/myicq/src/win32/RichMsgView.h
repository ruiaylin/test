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

#if !defined(AFX_RICHMSGVIEW_H__8C2D6D5F_E2B7_4709_8934_B8AB604D0B54__INCLUDED_)
#define AFX_RICHMSGVIEW_H__8C2D6D5F_E2B7_4709_8934_B8AB604D0B54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichMsgView.h : header file
//

#include "RichEditCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////
// CRichMsgView window

class CRichMsgView : public CRichEditCtrlEx
{
// Construction
public:
	CRichMsgView();

// Attributes
public:

// Operations
public:
	void appendMsg(IcqMsg &msg, COLORREF color);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichMsgView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRichMsgView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRichMsgView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHMSGVIEW_H__8C2D6D5F_E2B7_4709_8934_B8AB604D0B54__INCLUDED_)
