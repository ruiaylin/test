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

#if !defined(AFX_CONFIGDLG_H__C9112F8F_EFB4_4DCA_B57F_90B20F1D27BA__INCLUDED_)
#define AFX_CONFIGDLG_H__C9112F8F_EFB4_4DCA_B57F_90B20F1D27BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

class CConfigDlg : public CDialog
{
// Construction
public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_CONFIG };
	BOOL	m_enable;
	CString	m_text;
	CTime	m_time;
	CTime	m_date;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H__C9112F8F_EFB4_4DCA_B57F_90B20F1D27BA__INCLUDED_)
