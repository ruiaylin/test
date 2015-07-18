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

#if !defined(AFX_PICCOMBOBOX_H__A7930041_D18D_4F5D_A74D_66884A98377A__INCLUDED_)
#define AFX_PICCOMBOBOX_H__A7930041_D18D_4F5D_A74D_66884A98377A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PicComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPicComboBox window

class CPicComboBox : public CComboBoxEx
{
// Construction
public:
	CPicComboBox();

// Attributes
public:

// Operations
public:
	void addPics();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPicComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPicComboBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICCOMBOBOX_H__A7930041_D18D_4F5D_A74D_66884A98377A__INCLUDED_)
