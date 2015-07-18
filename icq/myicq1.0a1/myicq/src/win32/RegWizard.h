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

#if !defined(AFX_REGWIZARD_H__99372F33_BF23_49A6_BA55_6DC376D7397D__INCLUDED_)
#define AFX_REGWIZARD_H__99372F33_BF23_49A6_BA55_6DC376D7397D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegWizard.h : header file
//

#include "icqwindow.h"
#include "RegModeDlg.h"
#include "RegBasicDlg.h"
#include "RegCommDlg.h"
#include "RegMiscDlg.h"
#include "RegNetworkDlg.h"
#include "RegFinishDlg.h"

class IcqUser;
class IcqOption;

/////////////////////////////////////////////////////////////////////////////
// CRegWizard

class CRegWizard : public CPropertySheet, public IcqWindow
{
	DECLARE_DYNAMIC(CRegWizard)

// Construction
public:
	CRegWizard();

	void onHostFound(in_addr &addr) {
		finishDlg.onHostFound(addr);
	}
	void onConnect(bool success) {
		finishDlg.onConnect(success);
	}
	void onLoginReply(uint16 error) {
		finishDlg.onLoginReply(error);
	}
	void onNewUINReply(QID &qid) {
		finishDlg.onNewUINReply(qid);
	}
	void onContactListReply() {
		finishDlg.onContactListReply();
	}
	void onRemoteContactList(const char *domain) {
		finishDlg.onRemoteContactList(domain);
	}
	void onSendError(uint32 seq) {
		finishDlg.onSendError(seq);
	}

// Attributes
public:
	BOOL isFinished;

	CRegModeDlg modeDlg;
	CRegBasicDlg basicDlg;
	CRegCommDlg commDlg;
	CRegMiscDlg miscDlg;
	CRegNetworkDlg networkDlg;
	CRegFinishDlg finishDlg;

// Operations
public:
	void getData(IcqUser *info, IcqOption *options = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRegWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRegWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGWIZARD_H__99372F33_BF23_49A6_BA55_6DC376D7397D__INCLUDED_)
