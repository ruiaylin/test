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

#if !defined(AFX_SENDREQUESTDLG_H__39A5FB7C_D28A_4E7A_B16E_F5BEA7D24051__INCLUDED_)
#define AFX_SENDREQUESTDLG_H__39A5FB7C_D28A_4E7A_B16E_F5BEA7D24051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendRequestDlg.h : header file
//

#include "MyDlg.h"
#include "icqwindow.h"
#include "BtnST.h"

class IcqContact;

/////////////////////////////////////////////////////////////////////////////
// CSendRequestDlg dialog

class CSendRequestDlg : public CMyDlg, public IcqWindow
{
// Construction
public:
	CSendRequestDlg(const char *name, IcqContact *c, CWnd* pParent = NULL);   // standard constructor
	CSendRequestDlg(IcqMsg *msg, CWnd *pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CSendRequestDlg)
	enum { IDD = IDD_SEND_REQUEST };
	CEdit	m_textEdit;
	CButtonST	m_faceButton;
	CString	m_nick;
	CString	m_qid;
	CString	m_text;
	//}}AFX_DATA

private:
	virtual void onAck(uint32 seq);
	void init();

	CString sessionName;
	uint16 port;
	IcqContact *contact;
	BOOL isSend;
	int frame;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendRequestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendRequestDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDREQUESTDLG_H__39A5FB7C_D28A_4E7A_B16E_F5BEA7D24051__INCLUDED_)
