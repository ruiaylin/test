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

// RichMsgView.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RichMsgView.h"
#include "icqlink.h"
#include "icqgroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRichMsgView

CRichMsgView::CRichMsgView()
{
}

CRichMsgView::~CRichMsgView()
{
}

void CRichMsgView::appendMsg(IcqMsg &msg, COLORREF color)
{
	const char *nick = "";

	if (!msg.id) {
		IcqContact *c = icqLink->findContact(msg.qid);
		if (c)
			nick = c->nick.c_str();
	} else {
		IcqGroup *g = icqLink->findGroup(msg.id);
		if (g) {
			GroupMember *m = g->getMemberInfo(msg.qid.uin);
			if (m)
				nick = m->nick.c_str();
		}
	}

	long start, end;
	SetSel(-1, -1);
	GetSel(start, end);

	CHARFORMAT defFormat;
	defFormat.cbSize = sizeof(defFormat);
	defFormat.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_FACE | CFM_SIZE | CFM_COLOR;
	GetDefaultCharFormat(defFormat);

	PARAFORMAT pf;
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT;
	pf.dxStartIndent = 0;
	SetParaFormat(pf);

	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);

	CString text;
	CString strTime = CTime(msg.when).Format("%H:%M:%S");
	text.Format("%s (%s): \r\n",
		(msg.flags & MF_RECEIVED) ? nick : icqLink->myInfo.nick.c_str(),
		(LPCTSTR) strTime);

	ReplaceSel(text);
	SetSel(start, -1);
	cf = defFormat;
	cf.crTextColor = color;
	cf.dwEffects = 0;
	SetSelectionCharFormat(cf);

	SetSel(-1, -1);
	insertMsg(msg);

	pf.dxStartIndent = 200;
	SetParaFormat(pf);

	uint8 mask = (MF_RELAY | MF_RECEIVED);
	if ((msg.flags & mask) == mask) {
		GetSel(start, end);

		text.LoadString(IDS_SERVER_RELAY);
		ReplaceSel("\r\n");
		ReplaceSel(text);
		SetSel(start, -1);

		cf = defFormat;
		cf.crTextColor = color;
		cf.dwEffects = 0;
		SetSelectionCharFormat(cf);
	}

	SetSel(-1, -1);
	ReplaceSel("\r\n");

	scrollToBottom();
}

BEGIN_MESSAGE_MAP(CRichMsgView, CRichEditCtrlEx)
	//{{AFX_MSG_MAP(CRichMsgView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichMsgView message handlers
