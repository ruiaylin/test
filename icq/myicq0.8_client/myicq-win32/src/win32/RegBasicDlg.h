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

#if !defined(AFX_REGBASICDLG_H__558DF5A4_F799_4B52_B28A_2F8ED999A38E__INCLUDED_)
#define AFX_REGBASICDLG_H__558DF5A4_F799_4B52_B28A_2F8ED999A38E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegBasicDlg.h : header file
//

#include "PicComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CRegBasicDlg dialog

class CRegBasicDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CRegBasicDlg)

// Construction
public:
	CRegBasicDlg();
	~CRegBasicDlg();

// Dialog Data
	//{{AFX_DATA(CRegBasicDlg)
	enum { IDD = IDD_REG_BASIC };
	CPicComboBox	m_cmbPic;
	CString	m_province;
	CString	m_city;
	CString	m_country;
	int		m_gender;
	CString	m_passwd;
	CString	m_passwdAgain;
	int		m_age;
	CString	m_nick;
	int		m_pic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegBasicDlg)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRegBasicDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGBASICDLG_H__558DF5A4_F799_4B52_B28A_2F8ED999A38E__INCLUDED_)
