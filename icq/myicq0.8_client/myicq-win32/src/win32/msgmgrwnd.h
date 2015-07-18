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

#if !defined(AFX_MSGMGRWND_H__C7051E17_AE0E_404F_AACB_0CFE47F10138__INCLUDED_)
#define AFX_MSGMGRWND_H__C7051E17_AE0E_404F_AACB_0CFE47F10138__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgMgrWnd.h : header file
//

#include "icqdb.h"

class CMsgTreeView;
class CMsgListView;
class CMsgView;

/////////////////////////////////////////////////////////////////////////////
// CMsgMgrWnd frame

class CMsgMgrWnd : public CFrameWnd, public DBSerialize
{
public:
	CMsgMgrWnd();

// Attributes
public:

// Operations
public:
	void ActivateFrame(DWORD uin = 0, int nCmdShow = -1);

	virtual void save(DBOutStream &out);
	virtual void load(DBInStream &in);

private:
	CToolBar toolBar;
	CSplitterWnd splitter, splitterRight;
	CMsgTreeView *treeView;
	CMsgListView *listView;
	CMsgView *msgView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgMgrWnd)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMsgMgrWnd();

	// Generated message map functions
	//{{AFX_MSG(CMsgMgrWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditDel();
	afx_msg void OnFileExit();
	afx_msg void OnExportTxt();
	afx_msg void OnViewBgcolor();
	afx_msg void OnViewFont();
	afx_msg void OnUpdateExportContact(CCmdUI* pCmdUI);
	afx_msg void OnExportContact();
	afx_msg void OnUpdateEditDel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFont(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportBak(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnEditSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGMGRWND_H__C7051E17_AE0E_404F_AACB_0CFE47F10138__INCLUDED_)
