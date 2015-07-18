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

#if !defined(AFX_VIEWMSGDLG_H__8C551F48_FF42_4D67_A062_3525E697D698__INCLUDED_)
#define AFX_VIEWMSGDLG_H__8C551F48_FF42_4D67_A062_3525E697D698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewMsgDlg.h : header file
//

#include "icqwindow.h"
#include "MyDlg.h"
#include "BtnST.h"
#include "HistoryListBox.h"

class CSendMsgDlg;

/////////////////////////////////////////////////////////////////////////////
// CViewMsgDlg dialog

class CViewMsgDlg : public CMyDlg, public IcqWindow
{
// Construction
public:
	CViewMsgDlg(IcqContact *contact, CWnd* pParent = NULL);   // standard constructor

	void onRecvMessage();

// Dialog Data
	//{{AFX_DATA(CViewMsgDlg)
	enum { IDD = IDD_VIEW_MESSAGE };
	CHistoryListBox	m_lstHistory;
	CButtonST	m_btnPic;
	//}}AFX_DATA

private:
	CSendMsgDlg *createSendMsgDlg();
	void expand();
	void shrink();

	IcqContact *contact;
	CSize wholeSize;
	BOOL expanded;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewMsgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNextMsg();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnMenuQuickReply();
	afx_msg void OnQuickReply();
	afx_msg void OnPic();
	afx_msg void OnHistory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWMSGDLG_H__8C551F48_FF42_4D67_A062_3525E697D698__INCLUDED_)
