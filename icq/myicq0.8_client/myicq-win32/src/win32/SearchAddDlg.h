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

#if !defined(AFX_SEARCHADDDLG_H__8ADABA11_75D3_4216_B37C_D0D6A96588D9__INCLUDED_)
#define AFX_SEARCHADDDLG_H__8ADABA11_75D3_4216_B37C_D0D6A96588D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchAddDlg.h : header file
//

#include "icqtypes.h"
#include "AnimPic.h"

/////////////////////////////////////////////////////////////////////////////
// CSearchAddDlg dialog

class CSearchAddDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSearchAddDlg)

// Construction
public:
	CSearchAddDlg();
	~CSearchAddDlg();

	void onAddFriendReply(uint8 result);
	void onRecvMessage(uint8 type, const char *text);
	void onAck(uint32 seq);

// Dialog Data
	//{{AFX_DATA(CSearchAddDlg)
	enum { IDD = IDD_SEARCH_ADD };
	CAnimPic	m_ctlAnim;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSearchAddDlg)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSearchAddDlg)
	afx_msg void OnSendRequest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHADDDLG_H__8ADABA11_75D3_4216_B37C_D0D6A96588D9__INCLUDED_)
