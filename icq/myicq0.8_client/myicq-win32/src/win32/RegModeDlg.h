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

#if !defined(AFX_REGMODEDLG_H__139045C9_A391_46E0_8245_30E942D0382D__INCLUDED_)
#define AFX_REGMODEDLG_H__139045C9_A391_46E0_8245_30E942D0382D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegModeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegModeDlg dialog

class CRegModeDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CRegModeDlg)

// Construction
public:
	CRegModeDlg();
	~CRegModeDlg();

	void enableControls(BOOL enable = TRUE);

// Dialog Data
	//{{AFX_DATA(CRegModeDlg)
	enum { IDD = IDD_REG_MODE };
	int		m_mode;
	UINT	m_uin;
	CString	m_passwd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegModeDlg)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRegModeDlg)
	afx_msg void OnNewUin();
	afx_msg void OnExistingUin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGMODEDLG_H__139045C9_A391_46E0_8245_30E942D0382D__INCLUDED_)
