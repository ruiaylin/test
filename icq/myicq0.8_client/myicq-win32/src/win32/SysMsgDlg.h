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

#if !defined(AFX_SYSMSGDLG_H__CEAEE224_3F13_4AC4_9093_CB86AF5F22C9__INCLUDED_)
#define AFX_SYSMSGDLG_H__CEAEE224_3F13_4AC4_9093_CB86AF5F22C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SysMsgDlg.h : header file
//

#include "icqwindow.h"
#include "MyDlg.h"
#include "AnimButton.h"

class IcqMsg;

/////////////////////////////////////////////////////////////////////////////
// CSysMsgDlg dialog

class CSysMsgDlg : public CMyDlg, public IcqWindow
{
// Construction
public:
	CSysMsgDlg(IcqMsg *msg, BOOL autoDelete = TRUE, CWnd* pParent = NULL);   // standard constructor
	CSysMsgDlg(uint32 uin, CWnd* pParent = NULL);
	~CSysMsgDlg();

	void onAddFriendReply(uint8 result);
	void onAck(uint32 seq);
	void onSendError(uint32 seq);

// Dialog Data
	//{{AFX_DATA(CSysMsgDlg)
	enum { IDD = IDD_SYS_MESSAGE };
	CEdit	m_edtMsg;
	CAnimButton	m_btnPic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

private:
	void expand();
	void shrink();
	void enableControls(BOOL enable);

	IcqMsg *msg;
	BOOL autoDelete;
	BOOL expanded;
	CSize wholeSize;
	int lowerHeight;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSysMsgDlg)
	afx_msg void OnPic();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnReqAccept();
	afx_msg void OnReqReject();
	afx_msg void OnSendRequest();
	afx_msg void OnAddFriend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSMSGDLG_H__CEAEE224_3F13_4AC4_9093_CB86AF5F22C9__INCLUDED_)
