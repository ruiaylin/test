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

#if !defined(AFX_MSGVIEW_H__D17D6457_6B67_450A_BDBF_3914741E85CC__INCLUDED_)
#define AFX_MSGVIEW_H__D17D6457_6B67_450A_BDBF_3914741E85CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "RichEditCtrlEx.h"

class IcqMsg;
class DBOutStream;
class DBInStream;

class CMsgView : public CFormView {
protected:
	CMsgView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMsgView)

// Form Data
public:
	//{{AFX_DATA(CMsgView)
	enum { IDD = IDD_MSG_VIEW };
	CRichEditCtrlEx	m_msgEdit;
	CStatic	m_sendFace;
	CStatic	m_recvFace;
	CString	m_date;
	CString	m_receiver;
	CString	m_sender;
	CString	m_time;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void save(DBOutStream &out);
	void load(DBInStream &in);

	void showMsg(IcqMsg *msg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMsgView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMsgView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateViewFont(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnRclickMsgEdit(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGVIEW_H__D17D6457_6B67_450A_BDBF_3914741E85CC__INCLUDED_)
