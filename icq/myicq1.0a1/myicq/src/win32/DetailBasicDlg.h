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

#if !defined(AFX_DETAILBASICDLG_H__6EE5C1B7_132B_4E63_A940_3252ABD93071__INCLUDED_)
#define AFX_DETAILBASICDLG_H__6EE5C1B7_132B_4E63_A940_3252ABD93071__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetailBasicDlg.h : header file
//

#include "icqtypes.h"

class IcqInfo;

/////////////////////////////////////////////////////////////////////////////
// CDetailBasicDlg dialog

class IcqContact;

class CDetailBasicDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CDetailBasicDlg)

// Construction
public:
	CDetailBasicDlg();
	~CDetailBasicDlg();

	void enableAll(BOOL enable);
	void setData(IcqInfo *info);

// Dialog Data
	//{{AFX_DATA(CDetailBasicDlg)
	enum { IDD = IDD_DETAIL_BASIC };
	CComboBox	m_provCombo;
	CComboBox	m_countryCombo;
	CComboBox	m_genderCombo;
	CComboBoxEx	m_faceCombo;
	int		m_age;
	int		m_gender;
	CString	m_nick;
	int		m_pic;
	CString	m_country;
	CString	m_city;
	CString	m_province;
	CString	m_qid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDetailBasicDlg)
	public:
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDetailBasicDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILBASICDLG_H__6EE5C1B7_132B_4E63_A940_3252ABD93071__INCLUDED_)
