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

#if !defined(AFX_MODIFYDETAILDLG_H__50422B95_1A70_40B8_8268_2A944F52D9DE__INCLUDED_)
#define AFX_MODIFYDETAILDLG_H__50422B95_1A70_40B8_8268_2A944F52D9DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyDetailDlg.h : header file
//

#include "icqwindow.h"
#include "DetailBasicDlg.h"
#include "DetailCommDlg.h"
#include "DetailMiscDlg.h"
#include "DetailAuthDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CModifyDetailDlg

class CModifyDetailDlg : public CPropertySheet, public IcqWindow
{
	DECLARE_DYNAMIC(CModifyDetailDlg)

// Construction
public:
	CModifyDetailDlg();

	void onAck(uint32 seq);
	void onSendError(uint32 seq);

	BOOL Create(CWnd *pParentWnd = NULL);
	void setData();
	void getData(IcqUser &user);
	void enableAll(BOOL enable = TRUE);

// Attributes
private:
	CDetailBasicDlg basicPage;
	CDetailCommDlg commPage;
	CDetailMiscDlg miscPage;
	CDetailAuthDlg authPage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyDetailDlg)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModifyDetailDlg();

	// Generated message map functions
protected:
	void OnModify();

	//{{AFX_MSG(CModifyDetailDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYDETAILDLG_H__50422B95_1A70_40B8_8268_2A944F52D9DE__INCLUDED_)
