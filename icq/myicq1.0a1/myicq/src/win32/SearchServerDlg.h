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

#if !defined(AFX_SEARCHSERVERDLG_H__AFD1FDC1_9E9E_464E_AEA0_08B2D6B81CB3__INCLUDED_)
#define AFX_SEARCHSERVERDLG_H__AFD1FDC1_9E9E_464E_AEA0_08B2D6B81CB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchServerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchServerDlg dialog

struct SERVER_INFO;

class CSearchServerDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSearchServerDlg)

// Construction
public:
	CSearchServerDlg();
	~CSearchServerDlg();

	void getServer(CString &name);

	void onServerListReply(SERVER_INFO info[], int n);

// Dialog Data
	//{{AFX_DATA(CSearchServerDlg)
	enum { IDD = IDD_SEARCH_SERVER };
	CListCtrl	m_serverList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSearchServerDlg)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSearchServerDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHSERVERDLG_H__AFD1FDC1_9E9E_464E_AEA0_08B2D6B81CB3__INCLUDED_)
