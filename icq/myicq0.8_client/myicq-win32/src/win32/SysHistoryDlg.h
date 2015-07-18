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

#if !defined(AFX_SYSHISTORYDLG_H__E8F1276D_547A_4023_B41E_E066EC41A377__INCLUDED_)
#define AFX_SYSHISTORYDLG_H__E8F1276D_547A_4023_B41E_E066EC41A377__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SysHistoryDlg.h : header file
//

#include "icqwindow.h"
#include "MyDlg.h"
#include "ListCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////
// CSysHistoryDlg dialog

class CSysHistoryDlg : public CMyDlg, public IcqWindow
{
// Construction
public:
	CSysHistoryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSysHistoryDlg)
	enum { IDD = IDD_SYS_HISTORY };
	CListCtrlEx	m_ctlHistory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysHistoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

private:
	void deleteAllItems();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSysHistoryDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkHistory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDelete();
	afx_msg void OnDeleteAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSHISTORYDLG_H__E8F1276D_547A_4023_B41E_E066EC41A377__INCLUDED_)
