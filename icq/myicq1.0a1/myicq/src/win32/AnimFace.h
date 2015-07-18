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

#if !defined(AFX_ANIMFACE_H__03EC488B_5B00_4CB8_B7F8_053BBCE424B8__INCLUDED_)
#define AFX_ANIMFACE_H__03EC488B_5B00_4CB8_B7F8_053BBCE424B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimFace.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnimFace window

class CAnimFace : public CStatic
{
// Construction
public:
	CAnimFace();

// Attributes
public:

// Operations
public:
	void start();
	void stop();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimFace)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnimFace();

private:
	int frame;

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnimFace)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMFACE_H__03EC488B_5B00_4CB8_B7F8_053BBCE424B8__INCLUDED_)
