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

#if !defined(AFX_GROUPMSGDLG_H__50E60DD4_1EF4_416C_8FDC_DEA12037FC1C__INCLUDED_)
#define AFX_GROUPMSGDLG_H__50E60DD4_1EF4_416C_8FDC_DEA12037FC1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupMsgDlg.h : header file
//

#include "icqwindow.h"
#include "MyDlg.h"
#include "FORMATBA.H"
#include "MsgEdit.h"
#include "RichMsgView.h"
#include "BtnST.h"
#include "icqclient.h"

class IcqGroup;

/////////////////////////////////////////////////////////////////////////////
// CGroupMsgDlg dialog

class CGroupMsgDlg : public CMyDlg, public GroupWindow
{
// Construction
public:
	CGroupMsgDlg(IcqGroup *g, CWnd* pParent = NULL);   // standard constructor

	virtual void onAck();
	virtual void onSendError();
	
	void onRecvMessage(IcqMsg &msg);

// Dialog Data
	//{{AFX_DATA(CGroupMsgDlg)
	enum { IDD = IDD_GROUP_MSG };
	CButtonST	m_groupButton;
	CRichMsgView	m_msgView;
	CMsgEdit	m_msgEdit;
	//}}AFX_DATA

private:
	BOOL CreateFormatBar();
	void enableControls(BOOL enable = TRUE);
	void fillMsg(IcqMsg &msg, LPCTSTR text);
	void sendMessage(IcqMsg &msg);

	CFormatBar m_wndFormatBar;
	IcqGroup *group;
	IcqMsg msgSent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupMsgDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGroupMsgDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSend();
	afx_msg void OnGroupDetail();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPMSGDLG_H__50E60DD4_1EF4_416C_8FDC_DEA12037FC1C__INCLUDED_)
