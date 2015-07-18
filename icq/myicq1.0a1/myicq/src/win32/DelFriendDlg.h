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

#if !defined(AFX_DELFRIENDDLG_H__3625EB0D_357F_4E14_9F37_144299F592E1__INCLUDED_)
#define AFX_DELFRIENDDLG_H__3625EB0D_357F_4E14_9F37_144299F592E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelFriendDlg.h : header file
//

#include "icqwindow.h"
#include "MyDlg.h"
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CDelFriendDlg dialog

class CDelFriendDlg : public CMyDlg, public IcqWindow
{
// Construction
public:
	CDelFriendDlg(QID &qid, CWnd* pParent = NULL);   // standard constructor

	void onAck(uint32 seq);
	void onSendError(uint32 seq);

// Dialog Data
	//{{AFX_DATA(CDelFriendDlg)
	enum { IDD = IDD_DEL_FRIEND };
	CButtonST	m_faceButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelFriendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelFriendDlg)
	afx_msg void OnDelFriend();
	virtual BOOL OnInitDialog();
	afx_msg void OnFace();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELFRIENDDLG_H__3625EB0D_357F_4E14_9F37_144299F592E1__INCLUDED_)
