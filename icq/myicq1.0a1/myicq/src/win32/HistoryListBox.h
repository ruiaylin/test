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

#if !defined(AFX_HISTORYLISTBOX_H__42CCCD8D_8050_446E_9B76_652D2E202745__INCLUDED_)
#define AFX_HISTORYLISTBOX_H__42CCCD8D_8050_446E_9B76_652D2E202745__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistoryListBox.h : header file
//

#include "icqclient.h"

/////////////////////////////////////////////////////////////////////////////
// CHistoryListBox window

class CHistoryListBox : public CListBox
{
// Construction
public:
	CHistoryListBox();

// Attributes
public:

// Operations
public:
	BOOL loadHistory(QID &qid);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHistoryListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHistoryListBox)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRecordCopy();
	afx_msg void OnRecordDel();
	afx_msg void OnRecordCut();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	QID qid;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTORYLISTBOX_H__42CCCD8D_8050_446E_9B76_652D2E202745__INCLUDED_)
