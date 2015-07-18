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

#if !defined(AFX_SENDFILEDLG_H__49BD1314_8BC6_4486_96F0_B30C36DB7A45__INCLUDED_)
#define AFX_SENDFILEDLG_H__49BD1314_8BC6_4486_96F0_B30C36DB7A45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendFileDlg.h : header file
//

#include "filesession.h"

class IcqLinkBase;
class ContactInfo;

/////////////////////////////////////////////////////////////////////////////
// CSendFileDlg dialog

class CSendFileDlg : public CDialog, public FileSessionListener
{
// Construction
public:
	CSendFileDlg(FileSession *, CWnd* pParent = NULL);   // standard constructor
	~CSendFileDlg();

	virtual const char *getPathName(const char *name, uint32 size);
	virtual void onFileReceive();
	virtual void onFileProgress(uint32 bytes);
	virtual void onFileFinished();

// Dialog Data
	//{{AFX_DATA(CSendFileDlg)
	enum { IDD = IDD_SEND_FILE };
	CStatic	m_faceStatic;
	CProgressCtrl	m_ctlProgress;
	CString	m_uin;
	CString	m_nick;
	//}}AFX_DATA

private:
	IcqLinkBase *icqLink;
	ContactInfo *contact;

	FileSession *session;
	CString pathName;
	CString fileName;
	uint32 fileSize;

	uint32 bytesSent;
	uint32 lastBytes;
	DWORD lastTime;
	int frame;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDFILEDLG_H__49BD1314_8BC6_4486_96F0_B30C36DB7A45__INCLUDED_)
