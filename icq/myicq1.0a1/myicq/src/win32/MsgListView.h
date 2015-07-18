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

#if !defined(AFX_MSGLISTVIEW_H__838E7890_C9D6_4580_B44F_7A408FB9C890__INCLUDED_)
#define AFX_MSGLISTVIEW_H__838E7890_C9D6_4580_B44F_7A408FB9C890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgListView.h : header file
//

#include "icqtypes.h"

class CMsgView;
class DBOutStream;
class DBInStream;

/////////////////////////////////////////////////////////////////////////////
// CMsgListView view

class CMsgListView : public CListView
{
protected:
	CMsgListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMsgListView)

// Attributes
public:
	void setMsgView(CMsgView *listener) {
		msgView = listener;
	}

// Operations
public:
	void save(DBOutStream &out);
	void load(DBInStream &in);

	void clear();
	void showMsg(QID *qid);
	void delSelectedMsg();
	void exportTxt(CStdioFile &file);

private:
	CMsgView *msgView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgListView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMsgListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditDel(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


void exportMsgTxt(CStdioFile &file, IcqMsg *msg, const char *nick);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGLISTVIEW_H__838E7890_C9D6_4580_B44F_7A408FB9C890__INCLUDED_)
