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

#if !defined(AFX_DETAILCOMMDLG_H__4A352E0C_32EB_4FFB_B894_8BD035896128__INCLUDED_)
#define AFX_DETAILCOMMDLG_H__4A352E0C_32EB_4FFB_B894_8BD035896128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetailCommDlg.h : header file
//

class IcqInfo;

/////////////////////////////////////////////////////////////////////////////
// CDetailCommDlg dialog

class CDetailCommDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CDetailCommDlg)

// Construction
public:
	CDetailCommDlg();
	~CDetailCommDlg();

	void enableAll(BOOL enable);
	void setData(IcqInfo *info);

// Dialog Data
	//{{AFX_DATA(CDetailCommDlg)
	enum { IDD = IDD_DETAIL_COMM };
	CString	m_address;
	CString	m_email;
	CString	m_tel;
	CString	m_zipcode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDetailCommDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDetailCommDlg)
	virtual void OnCancel();
	afx_msg void OnUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILCOMMDLG_H__4A352E0C_32EB_4FFB_B894_8BD035896128__INCLUDED_)
