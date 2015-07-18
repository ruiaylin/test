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

#if !defined(AFX_SEARCHCUSTOMDLG_H__3554B349_B196_4B71_AB22_A9288D044673__INCLUDED_)
#define AFX_SEARCHCUSTOMDLG_H__3554B349_B196_4B71_AB22_A9288D044673__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchCustomDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchCustomDlg dialog

class CSearchCustomDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSearchCustomDlg)

// Construction
public:
	CSearchCustomDlg();
	~CSearchCustomDlg();

// Dialog Data
	//{{AFX_DATA(CSearchCustomDlg)
	enum { IDD = IDD_SEARCH_CUSTOM };
	CString	m_email;
	CString	m_nick;
	UINT	m_uin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSearchCustomDlg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSearchCustomDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHCUSTOMDLG_H__3554B349_B196_4B71_AB22_A9288D044673__INCLUDED_)
