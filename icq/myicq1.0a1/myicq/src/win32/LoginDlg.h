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

#if !defined(AFX_LOGINDLG_H__122B52F6_CE02_4F48_8EBC_4C34F04804B6__INCLUDED_)
#define AFX_LOGINDLG_H__122B52F6_CE02_4F48_8EBC_4C34F04804B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginDlg.h : header file
//

#include "MyDlg.h"
#include "icqclient.h"

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CMyDlg
{
// Construction
public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoginDlg)
	enum { IDD = IDD_LOGIN };
	CComboBox	m_cmbUser;
	BOOL	m_invis;
	CString	m_passwd;
	BOOL	m_storePasswd;
	//}}AFX_DATA

	QID m_qid;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	IcqOption options;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginDlg)
	afx_msg void OnRegWizard();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeUser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINDLG_H__122B52F6_CE02_4F48_8EBC_4C34F04804B6__INCLUDED_)
