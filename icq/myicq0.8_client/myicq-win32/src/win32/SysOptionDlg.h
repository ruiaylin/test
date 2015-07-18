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

#if !defined(AFX_SYSOPTIONDLG_H__DA702293_7CAF_4EC9_A86D_FD2F79A6DDA5__INCLUDED_)
#define AFX_SYSOPTIONDLG_H__DA702293_7CAF_4EC9_A86D_FD2F79A6DDA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SysOptionDlg.h : header file
//

#include "OptionGeneralDlg.h"
#include "OptionReplyDlg.h"
#include "OptionNetworkDlg.h"
#include "OptionSoundDlg.h"
#include "OptionPluginDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSysOptionDlg

class CSysOptionDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CSysOptionDlg)

// Construction
public:
	CSysOptionDlg(CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysOptionDlg)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSysOptionDlg();

private:
	COptionGeneralDlg generalDlg;
	COptionReplyDlg replyDlg;
	COptionNetworkDlg networkDlg;
	COptionSoundDlg soundDlg;
	COptionPluginDlg pluginDlg;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSysOptionDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSOPTIONDLG_H__DA702293_7CAF_4EC9_A86D_FD2F79A6DDA5__INCLUDED_)
