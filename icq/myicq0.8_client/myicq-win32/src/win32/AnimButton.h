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

#if !defined(AFX_ANIMBUTTON_H__53E5D1C1_D20C_437A_8F1C_7417B87A0C2D__INCLUDED_)
#define AFX_ANIMBUTTON_H__53E5D1C1_D20C_437A_8F1C_7417B87A0C2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimButton.h : header file
//

#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimButton window

class CAnimButton : public CButtonST
{
// Construction
public:
	CAnimButton();

// Attributes
public:
	void setImageList(CImageList *pImageList, int n = -1);
	
// Operations
public:
	void stop(int frame = 0);
	void start();

private:
	CImageList *pImageList;
	int imageCount;
	int frame;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnimButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnimButton)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMBUTTON_H__53E5D1C1_D20C_437A_8F1C_7417B87A0C2D__INCLUDED_)
