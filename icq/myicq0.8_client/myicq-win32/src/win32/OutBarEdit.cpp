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

// OutBarEdit.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "OutBarEdit.h"
#include "OutBarCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OutBarEdit

OutBarEdit::OutBarEdit(OutBarCtrl *l, int obj, int index)
: listener(l)
{
	this->obj = obj;
	this->index = index;
}

OutBarEdit::~OutBarEdit()
{
}


BEGIN_MESSAGE_MAP(OutBarEdit, CEdit)
	//{{AFX_MSG_MAP(OutBarEdit)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OutBarEdit message handlers

void OutBarEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	listener->onEditCanceled();	
}

BOOL OutBarEdit::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN) {
			listener->onEditFinished();
			return TRUE;
		}
		if (pMsg->wParam == VK_ESCAPE) {
			listener->onEditCanceled();
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}
