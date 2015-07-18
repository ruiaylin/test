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

#if !defined(AFX_SEARCHMODEDLG_H__7E663256_7AC9_495C_8119_163DD388D1B9__INCLUDED_)
#define AFX_SEARCHMODEDLG_H__7E663256_7AC9_495C_8119_163DD388D1B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchModeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchModeDlg dialog

class CSearchModeDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSearchModeDlg)

// Construction
public:
	CSearchModeDlg();
	~CSearchModeDlg();

// Dialog Data
	//{{AFX_DATA(CSearchModeDlg)
	enum { IDD = IDD_SEARCH_MODE };
	int		m_mode;
	CString	m_nrOnlines;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSearchModeDlg)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSearchModeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHMODEDLG_H__7E663256_7AC9_495C_8119_163DD388D1B9__INCLUDED_)
