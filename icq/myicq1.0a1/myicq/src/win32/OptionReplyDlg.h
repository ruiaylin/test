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

#if !defined(AFX_OPTIONREPLYDLG_H__4128E474_876B_449D_BFEC_9621DF817ECA__INCLUDED_)
#define AFX_OPTIONREPLYDLG_H__4128E474_876B_449D_BFEC_9621DF817ECA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionReplyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionReplyDlg dialog

class COptionReplyDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionReplyDlg)

// Construction
public:
	COptionReplyDlg();
	~COptionReplyDlg();

// Dialog Data
	//{{AFX_DATA(COptionReplyDlg)
	enum { IDD = IDD_OPTION_REPLY };
	CComboBox	m_cmbQuickReply;
	CComboBox	m_cmbAutoReply;
	BOOL	m_autoSwitchStatus;
	BOOL	m_autoCancelAway;
	int		m_autoStatusTime;
	int		m_autoStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionReplyDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void enableControls(BOOL enable);

	CStringList autoReplyList;
	CStringList quickReplyList;
	int curAutoReply;
	int curQuickReply;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionReplyDlg)
	afx_msg void OnAutoReply();
	virtual BOOL OnInitDialog();
	afx_msg void OnAddAutoReply();
	afx_msg void OnDelAutoReply();
	afx_msg void OnSelchangeAutoReply();
	afx_msg void OnAddQuickReply();
	afx_msg void OnDelQuickReply();
	afx_msg void OnSelchangeQuickReply();
	afx_msg void OnAutoSwitchStatus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONREPLYDLG_H__4128E474_876B_449D_BFEC_9621DF817ECA__INCLUDED_)
