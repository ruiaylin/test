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

// ChatEdit.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "ChatEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_LINES		10

/////////////////////////////////////////////////////////////////////////////
// CChatEdit

CChatEdit::CChatEdit()
{
}

CChatEdit::~CChatEdit()
{
}


BEGIN_MESSAGE_MAP(CChatEdit, CEdit)
	//{{AFX_MSG_MAP(CChatEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatEdit message handlers

void CChatEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_RETURN) {
		int line = (LineFromChar() + 1) % MAX_LINES;
		int start = LineIndex(line);
		if (start >= 0) {
			int end = LineIndex(line + 1);
			if (end > 2)
				end -= 2;
			SetSel(start, end);
			Clear();
			return;
		}
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
