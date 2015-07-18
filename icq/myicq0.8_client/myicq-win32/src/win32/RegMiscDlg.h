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

#if !defined(AFX_REGMISCDLG_H__27780679_F77F_41BF_87A2_4E180164772E__INCLUDED_)
#define AFX_REGMISCDLG_H__27780679_F77F_41BF_87A2_4E180164772E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegMiscDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegMiscDlg dialog

class CRegMiscDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CRegMiscDlg)

// Construction
public:
	CRegMiscDlg();
	~CRegMiscDlg();

// Dialog Data
	//{{AFX_DATA(CRegMiscDlg)
	enum { IDD = IDD_REG_MISC };
	int		m_blood;
	CString	m_college;
	CString	m_homepage;
	CString	m_intro;
	CString	m_profession;
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegMiscDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRegMiscDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGMISCDLG_H__27780679_F77F_41BF_87A2_4E180164772E__INCLUDED_)
