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

#if !defined(AFX_CHATDLG_H__DAFC1114_A397_4CEE_817E_4830C13DF6E8__INCLUDED_)
#define AFX_CHATDLG_H__DAFC1114_A397_4CEE_817E_4830C13DF6E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChatDlg.h : header file
//

#include "chatsession.h"
#include "ChatEdit.h"
#include "wave.h"

class TcpSessionBase;

/////////////////////////////////////////////////////////////////////////////
// CChatDlg dialog

class CChatDlg : public CDialog, public ChatSessionListener
{
// Construction
public:
	CChatDlg(ChatSession *s, CWnd* pParent = NULL);   // standard constructor
	~CChatDlg();

	virtual void onClose();
	virtual void onChatText(const char *text);
	virtual void onSpeechData(const char *frame, int n);

// Dialog Data
	//{{AFX_DATA(CChatDlg)
	enum { IDD = IDD_CHAT };
	CProgressCtrl	m_sendVol;
	CProgressCtrl	m_recvVol;
	CChatEdit	m_sendEdit;
	CStatic	m_contactFace;
	CStatic	m_myFace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

private:
	virtual void OnCancel();

	BOOL initSpeech();

	ChatSession *session;
	TcpSessionBase *tcp;
	IcqLinkBase *icqLink;
	CTime startTime;

	WaveIn waveIn;
	WaveOut waveOut;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeSendText();
	//}}AFX_MSG
	afx_msg LRESULT onWaveInData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onWaveOutDone(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHATDLG_H__DAFC1114_A397_4CEE_817E_4830C13DF6E8__INCLUDED_)
