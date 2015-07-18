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

#if !defined(AFX_VIEWDETAILDLG_H__2F00753A_16FD_4437_8E34_1A1B9F5AEDE6__INCLUDED_)
#define AFX_VIEWDETAILDLG_H__2F00753A_16FD_4437_8E34_1A1B9F5AEDE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewDetailDlg.h : header file
//

#include "icqwindow.h"
#include "DetailBasicDlg.h"
#include "DetailCommDlg.h"
#include "DetailMiscDlg.h"
#include "DetailCustomDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CViewDetailDlg

class IcqContact;

class CViewDetailDlg : public CPropertySheet, public IcqWindow
{
	DECLARE_DYNAMIC(CViewDetailDlg)

// Construction
public:
	CViewDetailDlg(uint32 uin);

// Attributes
public:
	IcqContact *contact;

private:
	CDetailBasicDlg basicPage;
	CDetailCommDlg commPage;
	CDetailMiscDlg miscPage;
	CDetailCustomDlg customPage;
	
// Operations
public:
	void onSendError(uint32 seq);

	void onUpdateContactReply(IcqContact *c);
	void enableButtons(BOOL enable = TRUE);

	BOOL Create(CWnd* pParentWnd = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewDetailDlg)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

private:
	void setData(IcqContact *contact);

// Implementation
public:
	virtual ~CViewDetailDlg();

	// Generated message map functions
protected:
	void OnUpdate();

	//{{AFX_MSG(CViewDetailDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWDETAILDLG_H__2F00753A_16FD_4437_8E34_1A1B9F5AEDE6__INCLUDED_)
