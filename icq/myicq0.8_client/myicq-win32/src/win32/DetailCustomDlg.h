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

#if !defined(AFX_DETAILCUSTOMDLG_H__8BF7DE93_8489_4155_AC61_C1678B15C71A__INCLUDED_)
#define AFX_DETAILCUSTOMDLG_H__8BF7DE93_8489_4155_AC61_C1678B15C71A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetailCustomDlg.h : header file
//

#include "icqclient.h"

/////////////////////////////////////////////////////////////////////////////
// CDetailCustomDlg dialog

class CDetailCustomDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CDetailCustomDlg)

// Construction
public:
	CDetailCustomDlg();
	~CDetailCustomDlg();

	void enableGreeting();

// Dialog Data
	//{{AFX_DATA(CDetailCustomDlg)
	enum { IDD = IDD_DETAIL_CUSTOM };
	CComboBox	m_cmbSound;
	CCheckListBox	m_lstOptions;
	//}}AFX_DATA

private:
	int curSel;
	CString soundFiles[NR_CUSTOM_SOUNDS];

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDetailCustomDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDetailCustomDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeOptions();
	afx_msg void OnCustomSound();
	afx_msg void OnSelchangeSound();
	afx_msg void OnPlay();
	afx_msg void OnBrowse();
	afx_msg void OnModify();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILCUSTOMDLG_H__8BF7DE93_8489_4155_AC61_C1678B15C71A__INCLUDED_)
