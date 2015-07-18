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

// HistoryListBox.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "HistoryListBox.h"
#include "GfxPopupMenu.h"
#include "icqlink.h"
#include "icqdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryListBox

CHistoryListBox::CHistoryListBox()
{
}

CHistoryListBox::~CHistoryListBox()
{
}

BOOL CHistoryListBox::loadHistory(QID &qid)
{
	this->qid = qid;

	IcqContact *c = icqLink->findContact(qid);
	if (!c)
		return FALSE;

	PtrList msgList;
	IcqDB::loadMsg(&qid, msgList, 100);

	SetRedraw(FALSE);

	while (!msgList.empty()) {
		IcqMsg *msg = (IcqMsg *) msgList.back();
		msgList.pop_back();

		CTime t(msg->when);
		CString strTime = t.Format("(%Y-%m-%d %H:%M:%S)");
		CString str, text;
		getMsgText(msg, text);
		str.Format("%s   %s\r\n%s", strTime,
			(msg->flags & MF_RECEIVED) ? c->nick.c_str() : icqLink->myInfo.nick.c_str(),
			(LPCTSTR) text);
		AddString(str);

		delete msg;
	}

	SetRedraw();
	Invalidate(FALSE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CHistoryListBox, CListBox)
	//{{AFX_MSG_MAP(CHistoryListBox)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_RECORD_COPY, OnRecordCopy)
	ON_COMMAND(ID_RECORD_DEL, OnRecordDel)
	ON_COMMAND(ID_RECORD_CUT, OnRecordCut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistoryListBox message handlers

void CHistoryListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
	int index = lpDrawItemStruct->itemID;
	if (index < 0)
		return;

	CRect rcItem(lpDrawItemStruct->rcItem);
	CString strText;
	GetText(index, strText);
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	COLORREF bgColor;
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState & ODS_SELECTED) &&
		(GetFocus() == this))
		bgColor = GetSysColor(COLOR_HIGHLIGHT);
	else
		bgColor = GetSysColor(COLOR_WINDOW);

	dc.FillSolidRect(rcItem, bgColor);

	COLORREF oldTextColor = dc.SetTextColor(RGB(255, 0, 0));
	int oldBkMode = dc.SetBkMode(TRANSPARENT);

	int i = strText.Find("\r\n");
	rcItem.top += dc.DrawText(strText, i, rcItem, DT_SINGLELINE);

	dc.SetTextColor(RGB(0, 0, 255));
	int j;
	++i;
	while ((j = strText.Find("\r\n", i)) >= 0) {
		rcItem.top += dc.DrawText(strText.Mid(i, j - i), rcItem, DT_SINGLELINE);
		i = j + 1;
	}
	dc.DrawText(strText.Right(strText.GetLength() - i), rcItem, DT_SINGLELINE);

	dc.SetBkMode(oldBkMode);
	dc.SetTextColor(oldTextColor);
	dc.Detach();
}

void CHistoryListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);
	CDC *pDC = GetDC();
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	ReleaseDC(pDC);

	CString strText;
	GetText(lpMeasureItemStruct->itemID, strText);
	int i = -2;
	int n = 0;
	lpMeasureItemStruct->itemHeight = tm.tmAscent;
	while (n++ < 10 && (i = strText.Find("\r\n", i + 2)) >= 0)
		lpMeasureItemStruct->itemHeight += tm.tmAscent;
}

void CHistoryListBox::OnRButtonDown(UINT nFlags, CPoint point) 
{
	BOOL out;
	int i = ItemFromPoint(point, out);
	if (out)
		return;

	SetCurSel(i);
	
	CMenu tmp;
	tmp.LoadMenu(IDR_RECORD);
	CGfxPopupMenu menu;
	menu.Attach(*tmp.GetSubMenu(0));

	menu.modifyMenu(ID_RECORD_COPY, IDB_COPY);
	menu.modifyMenu(ID_RECORD_CUT, IDB_CUT);
	menu.modifyMenu(ID_RECORD_DEL, IDB_DEL);

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(0, pt.x, pt.y, this);

	CListBox::OnRButtonDown(nFlags, point);
}

void CHistoryListBox::OnRecordCopy() 
{
	int i = GetCurSel();
	if (i < 0)
		return;

	CString str;
	GetText(i, str);

	if (OpenClipboard()) {
		EmptyClipboard();

		HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, str.GetLength() + 1);
		char *p = (char *) GlobalLock(hMem);
		if (p)
			lstrcpy(p, str);
		GlobalUnlock(hMem);

		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}
}

void CHistoryListBox::OnRecordDel() 
{
	int i = GetCurSel();
	if (i < 0)
		return;

	DeleteString(i);
	IcqDB::delMsg(&qid, i);
}

void CHistoryListBox::OnRecordCut() 
{
	OnRecordCopy();
	OnRecordDel();
}
