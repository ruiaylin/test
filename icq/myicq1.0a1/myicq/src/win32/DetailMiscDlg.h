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

#if !defined(AFX_DETAILMISCDLG_H__A4029A5E_CAFD_439A_A2D5_EB14E328CCDF__INCLUDED_)
#define AFX_DETAILMISCDLG_H__A4029A5E_CAFD_439A_A2D5_EB14E328CCDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetailMiscDlg.h : header file
//

class IcqInfo;

/////////////////////////////////////////////////////////////////////////////
// CDetailMiscDlg dialog

class CDetailMiscDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CDetailMiscDlg)

// Construction
public:
	CDetailMiscDlg();
	~CDetailMiscDlg();

	void enableAll(BOOL enable);
	void setData(IcqInfo *info);

// Dialog Data
	//{{AFX_DATA(CDetailMiscDlg)
	enum { IDD = IDD_DETAIL_MISC };
	CComboBox	m_profCombo;
	CComboBox	m_bloodCombo;
	int		m_blood;
	CString	m_college;
	CString	m_homepage;
	CString	m_intro;
	CString	m_profession;
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDetailMiscDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDetailMiscDlg)
	afx_msg void OnUpdate();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILMISCDLG_H__A4029A5E_CAFD_439A_A2D5_EB14E328CCDF__INCLUDED_)
