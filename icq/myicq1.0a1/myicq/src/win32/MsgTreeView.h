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

#if !defined(AFX_MSGTREEVIEW_H__B0909E4B_9D75_4DB6_8017_A5A9B27CDFFC__INCLUDED_)
#define AFX_MSGTREEVIEW_H__B0909E4B_9D75_4DB6_8017_A5A9B27CDFFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgTreeView.h : header file
//

class CMsgListView;
class DBOutStream;
class DBInStream;

/////////////////////////////////////////////////////////////////////////////
// CMsgTreeView view

class CMsgTreeView : public CTreeView
{
protected:
	CMsgTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMsgTreeView)

// Attributes
public:
	void setListView(CMsgListView *listener) {
		listView = listener;
	}
	QID *getQID();

// Operations
public:
	void save(DBOutStream &out);
	void load(DBInStream &in);

	void expand(QID *qid);
	void delSelectedMsg();
	void exportTxt(CStdioFile &file);
	void exportContact(CStdioFile &file);

private:
	void delMsg(HTREEITEM item);
	void exportTxt(CStdioFile &file, HTREEITEM item);
	void exportContact(CStdioFile &file, HTREEITEM item);
	void exportBak(const char *fileName, HTREEITEM item);
	HTREEITEM findItem(HTREEITEM item, QID &qid);

	CMsgListView *listView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgTreeView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMsgTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgTreeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditDel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportContact(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportBak(CCmdUI* pCmdUI);
	afx_msg void OnExportBak();
	afx_msg void OnFileImport();
	afx_msg void OnExportBackup();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGTREEVIEW_H__B0909E4B_9D75_4DB6_8017_A5A9B27CDFFC__INCLUDED_)
