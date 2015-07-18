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

#if !defined(AFX_BROADCASTDLG_H__7150E8C8_FEB2_438D_870B_87E82399EC4F__INCLUDED_)
#define AFX_BROADCASTDLG_H__7150E8C8_FEB2_438D_870B_87E82399EC4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BroadcastDlg.h : header file
//

#include "MyDlg.h"
#include "icqwindow.h"

/////////////////////////////////////////////////////////////////////////////
// CBroadcastDlg dialog

class CBroadcastDlg : public CMyDlg, public IcqWindow
{
// Construction
public:
	CBroadcastDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBroadcastDlg)
	enum { IDD = IDD_BROADCAST };
	CEdit	m_textEdit;
	CTime	m_expire;
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBroadcastDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

private:
	virtual void onAck(uint32 seq);
	virtual void onSendError(uint32 seq);

	void enableControls(BOOL enable);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBroadcastDlg)
	afx_msg void OnSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROADCASTDLG_H__7150E8C8_FEB2_438D_870B_87E82399EC4F__INCLUDED_)
