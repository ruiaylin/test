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

#if !defined(AFX_REGCOMMDLG_H__CB7D4B0B_7D27_409A_BB5C_C04B01FF9F08__INCLUDED_)
#define AFX_REGCOMMDLG_H__CB7D4B0B_7D27_409A_BB5C_C04B01FF9F08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegCommDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegCommDlg dialog

class CRegCommDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CRegCommDlg)

// Construction
public:
	CRegCommDlg();
	~CRegCommDlg();

// Dialog Data
	//{{AFX_DATA(CRegCommDlg)
	enum { IDD = IDD_REG_COMM };
	CString	m_address;
	CString	m_email;
	CString	m_tel;
	CString	m_zipcode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegCommDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRegCommDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGCOMMDLG_H__CB7D4B0B_7D27_409A_BB5C_C04B01FF9F08__INCLUDED_)
