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

#if !defined(AFX_REGFINISHDLG_H__9B4A1B54_08A2_42DB_9736_A89B502B077C__INCLUDED_)
#define AFX_REGFINISHDLG_H__9B4A1B54_08A2_42DB_9736_A89B502B077C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegFinishDlg.h : header file
//

#include "icqtypes.h"
#include "AnimPic.h"

/////////////////////////////////////////////////////////////////////////////
// CRegFinishDlg dialog

class CRegFinishDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CRegFinishDlg)

// Construction
public:
	CRegFinishDlg();
	~CRegFinishDlg();

	void onConnect(bool success);
	void onSendError(uint32 seq);
	void onNewUINReply(uint32 uin);
	void onContactListReply();
	void onLoginReply(uint8 error);
	void onTimeOut();
	void onHostFound(struct in_addr &addr);

// Dialog Data
	//{{AFX_DATA(CRegFinishDlg)
	enum { IDD = IDD_REG_FINISH };
	CAnimPic	m_anim;
	//}}AFX_DATA

private:
	void resolveHost();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegFinishDlg)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRegFinishDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGFINISHDLG_H__9B4A1B54_08A2_42DB_9736_A89B502B077C__INCLUDED_)
