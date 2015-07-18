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

#if !defined(AFX_MSGSEARCHDLG_H__C388A652_4A3D_4031_B8A9_A1D514610F5F__INCLUDED_)
#define AFX_MSGSEARCHDLG_H__C388A652_4A3D_4031_B8A9_A1D514610F5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgSearchDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgSearchDlg dialog

class CMsgSearchDlg : public CDialog
{
// Construction
public:
	CMsgSearchDlg(CWnd* pParent = NULL);   // standard constructor

	void onSearchMsg(DWORD uin, CString &nick);

// Dialog Data
	//{{AFX_DATA(CMsgSearchDlg)
	enum { IDD = IDD_MSG_SEARCH };
	CListCtrl	m_searchResult;
	CComboBox	m_contactCombo;
	int		m_searchMode;
	CString	m_searchText;
	//}}AFX_DATA

private:
	void searchMsg(DWORD uin, CString &nick);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMsgSearchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGSEARCHDLG_H__C388A652_4A3D_4031_B8A9_A1D514610F5F__INCLUDED_)
