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

#if !defined(AFX_LOGINAGAINDLG_H__8830278A_3620_401E_9C14_BA7D1B30BD25__INCLUDED_)
#define AFX_LOGINAGAINDLG_H__8830278A_3620_401E_9C14_BA7D1B30BD25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginAgainDlg.h : header file
//

#include "MyDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CLoginAgainDlg dialog

class CLoginAgainDlg : public CMyDlg
{
// Construction
public:
	CLoginAgainDlg(QID &qid, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoginAgainDlg)
	enum { IDD = IDD_LOGIN_AGAIN };
	CString	m_passwd;
	CString m_qid;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginAgainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginAgainDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINAGAINDLG_H__8830278A_3620_401E_9C14_BA7D1B30BD25__INCLUDED_)
