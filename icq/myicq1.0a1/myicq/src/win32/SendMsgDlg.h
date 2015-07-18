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

#if !defined(AFX_SENDMSGDLG_H__1C2AA9F5_40EE_416A_9BA5_6E0EF34A9B23__INCLUDED_)
#define AFX_SENDMSGDLG_H__1C2AA9F5_40EE_416A_9BA5_6E0EF34A9B23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendMsgDlg.h : header file
//

#include "MyDlg.h"
#include "icqwindow.h"
#include "BtnST.h"
#include "HistoryListBox.h"
#include "MsgEdit.h"
#include "RichMsgView.h"
#include "FORMATBA.H"

struct SEQ {
	QID *qid;
	uint32 seq;
};

/////////////////////////////////////////////////////////////////////////////
// CSendMsgDlg dialog

class CSendMsgDlg : public CMyDlg, public IcqWindow
{
// Construction
public:
	CSendMsgDlg(IcqContact *c, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendMsgDlg)
	enum { IDD = IDD_SEND_MESSAGE };
	CMsgEdit	m_msgEdit;
	CTreeCtrl	m_recpts;
	CHistoryListBox	m_lstHistory;
	CRichMsgView	m_msgView;
	CButtonST	m_btnPic;
	CString	m_qid;
	CString	m_ipport;
	CString	m_nick;
	CString	m_email;
	//}}AFX_DATA

public:
	virtual bool isSeq(uint32 seq);
	virtual void onAck(uint32 seq);
	virtual void onSendError(uint32 seq);

	void onRecvMessage(IcqMsg &msg);

	BOOL isDialogMode();
	void sendMessage(const char *text);
	void loadHistory();

private:
	BOOL CreateFormatBar();
	void fillMsg(IcqMsg &msg, const char *text);
	void enableControls(BOOL enable = TRUE);
	void setChatMode(BOOL moveControls = TRUE);
	void resize();
	void initTree();
	BOOL groupSend();
	void broadcastMsg();

	CFormatBar m_wndFormatBar;
	CHARFORMAT defFormat;
	CSize wholeSize;
	int upperHeight, lowerHeight;
	int frame;

	CImageList imageList;
	IcqContact *contact;
	IcqMsg msgSent;
	BOOL viewHistory;
	BOOL isMultiple;
	list<SEQ> seqList;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendMsgDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendMsgDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnFace();
	afx_msg void OnHistory();
	afx_msg void OnChatMode();
	afx_msg void OnEnter();
	afx_msg void OnCtrlEnter();
	afx_msg void OnSend();
	afx_msg void OnDblclkHistory();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGroupSend();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDMSGDLG_H__1C2AA9F5_40EE_416A_9BA5_6E0EF34A9B23__INCLUDED_)
