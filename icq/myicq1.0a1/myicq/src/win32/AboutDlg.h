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

#if !defined(AFX_ABOUTDLG_H__43C4645B_C041_43C3_BF17_C290D465AE8A__INCLUDED_)
#define AFX_ABOUTDLG_H__43C4645B_C041_43C3_BF17_C290D465AE8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutDlg.h : header file
//

#include "hyperlink.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

class CAboutDlg : public CDialog
{
// Construction
public:
	CAboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUT };
	CHyperLink	m_bugReportLabel;
	CHyperLink	m_thanksLabel;
	CHyperLink	m_ctlEmail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDLG_H__43C4645B_C041_43C3_BF17_C290D465AE8A__INCLUDED_)
