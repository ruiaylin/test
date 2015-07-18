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

#if !defined(AFX_OPTIONGENERALDLG_H__1A2B19B8_06FD_4F3D_81DA_43FFEAD18902__INCLUDED_)
#define AFX_OPTIONGENERALDLG_H__1A2B19B8_06FD_4F3D_81DA_43FFEAD18902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionGeneralDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionGeneralDlg dialog

class COptionGeneralDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionGeneralDlg)

// Construction
public:
	COptionGeneralDlg();
	~COptionGeneralDlg();

// Dialog Data
	//{{AFX_DATA(COptionGeneralDlg)
	enum { IDD = IDD_OPTION_GENERAL };
	CHotKeyCtrl	m_ctlHotKey;
	BOOL	m_alwaysOnTop;
	BOOL	m_taskbarIcon;
	BOOL	m_showOnlineOnly;
	BOOL	m_taskbarStyle;
	BOOL	m_colorfulURL;
	BOOL	m_ignoreStranger;
	BOOL	m_loginInvis;
	BOOL	m_monitorNetwork;
	BOOL	m_onlineNotify;
	BOOL	m_popupMsg;
	BOOL	m_useHotKey;
	int		m_defaultHotKey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionGeneralDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void enableControls(BOOL enable);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionGeneralDlg)
	afx_msg void OnUseHotkey();
	virtual BOOL OnInitDialog();
	afx_msg void OnHotkeyCustom();
	afx_msg void OnHotkeyDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONGENERALDLG_H__1A2B19B8_06FD_4F3D_81DA_43FFEAD18902__INCLUDED_)
