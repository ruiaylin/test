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

#if !defined(AFX_OPTIONPLUGINDLG_H__57C73C26_4C58_490C_89C1_CBDC64AEDDD7__INCLUDED_)
#define AFX_OPTIONPLUGINDLG_H__57C73C26_4C58_490C_89C1_CBDC64AEDDD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionPluginDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionPluginDlg dialog

class COptionPluginDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionPluginDlg)

// Construction
public:
	COptionPluginDlg();
	~COptionPluginDlg();

// Dialog Data
	//{{AFX_DATA(COptionPluginDlg)
	enum { IDD = IDD_OPTION_PLUGIN };
	CListCtrl	m_pluginList;
	//}}AFX_DATA

private:
	CImageList imageList;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionPluginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionPluginDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnConfig();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONPLUGINDLG_H__57C73C26_4C58_490C_89C1_CBDC64AEDDD7__INCLUDED_)
