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

#if !defined(AFX_OPTIONSOUNDDLG_H__591366CC_0AC3_4477_ABCB_AB8E7045061E__INCLUDED_)
#define AFX_OPTIONSOUNDDLG_H__591366CC_0AC3_4477_ABCB_AB8E7045061E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionSoundDlg.h : header file
//

#include "icqclient.h"

/////////////////////////////////////////////////////////////////////////////
// COptionSoundDlg dialog

class COptionSoundDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionSoundDlg)

// Construction
public:
	COptionSoundDlg();
	~COptionSoundDlg();

// Dialog Data
	//{{AFX_DATA(COptionSoundDlg)
	enum { IDD = IDD_OPTION_SOUND };
	CComboBox	m_cmbSoundEvent;
	int		m_soundOn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionSoundDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void enableControls(BOOL enable = TRUE);

	int curSel;
	CString soundFiles[NR_SOUNDS];

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionSoundDlg)
	afx_msg void OnBrowse();
	afx_msg void OnPlay();
	afx_msg void OnSelchangeSoundEvent();
	virtual BOOL OnInitDialog();
	afx_msg void OnSoundOn();
	afx_msg void OnSoundOff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSOUNDDLG_H__591366CC_0AC3_4477_ABCB_AB8E7045061E__INCLUDED_)
