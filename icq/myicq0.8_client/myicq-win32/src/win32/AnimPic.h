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

#if !defined(AFX_ANIMPIC_H__76930B09_72F2_4142_827C_723C3040F8E1__INCLUDED_)
#define AFX_ANIMPIC_H__76930B09_72F2_4142_827C_723C3040F8E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimPic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnimPic window

class CAnimPic : public CStatic
{
// Construction
public:
	CAnimPic();

// Attributes
public:

// Operations
public:
	void start();
	void stop();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimPic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnimPic();

private:
	int frame;

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnimPic)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMPIC_H__76930B09_72F2_4142_827C_723C3040F8E1__INCLUDED_)
