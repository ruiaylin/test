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

#if !defined(AFX_DETAILAUTHDLG_H__B91711CD_6BA7_4231_8964_8DE59AF10E65__INCLUDED_)
#define AFX_DETAILAUTHDLG_H__B91711CD_6BA7_4231_8964_8DE59AF10E65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetailAuthDlg.h : header file
//

class IcqUser;

/////////////////////////////////////////////////////////////////////////////
// CDetailAuthDlg dialog

class CDetailAuthDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CDetailAuthDlg)

// Construction
public:
	CDetailAuthDlg();
	~CDetailAuthDlg();

	BOOL UpdateData(BOOL bSaveAndValidate = TRUE);
	void enablePasswd(BOOL enable);
	void enableAll(BOOL enable);
	void setData(IcqUser *user);

// Dialog Data
	//{{AFX_DATA(CDetailAuthDlg)
	enum { IDD = IDD_DETAIL_AUTH };
	CString	m_passwd;
	CString	m_passwdAgain;
	CString	m_passwdOld;
	int		m_auth;
	BOOL	m_modifyPasswd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDetailAuthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDetailAuthDlg)
	afx_msg void OnModifyPasswd();
	afx_msg void OnModify();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILAUTHDLG_H__B91711CD_6BA7_4231_8964_8DE59AF10E65__INCLUDED_)
