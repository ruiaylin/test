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

// MsgListView.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "MsgListView.h"
#include "MsgView.h"
#include "icqlink.h"
#include "icqdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void exportMsgTxt(CStdioFile &file, IcqMsg *msg, const char *nick)
{
	CTime t((time_t) msg->when);
	CString str;
	CString text;
	getMsgText(msg, text);
	str.Format("%s %s  %s\n%s\n\n",
		t.Format("%Y-%m-%d"),
		t.Format("%H:%M:%S"),
		(msg->flags & MF_RECEIVED) ? nick : icqLink->myInfo.nick.c_str(),
		text
	);

	file.WriteString(str);
}

/////////////////////////////////////////////////////////////////////////////
// CMsgListView

IMPLEMENT_DYNCREATE(CMsgListView, CListView)

CMsgListView::CMsgListView()
{
	msgView = NULL;
}

CMsgListView::~CMsgListView()
{
}

void CMsgListView::save(DBOutStream &out)
{
	out << GetListCtrl().GetTextColor();
	out << GetListCtrl().GetBkColor();
}

void CMsgListView::load(DBInStream &in)
{
	COLORREF color;
	in >> color;
	GetListCtrl().SetTextColor(color);
	in >> color;
	GetListCtrl().SetBkColor(color);
}

void CMsgListView::clear()
{
	CListCtrl &ctrl = GetListCtrl();

	int n = ctrl.GetItemCount();
	for (int i = 0; i < n; ++i)
		delete (IcqMsg *) ctrl.GetItemData(i);

	ctrl.DeleteAllItems();
}

void CMsgListView::showMsg(DWORD uin)
{
	clear();

	if (uin > 0xfffffff0) {
		msgView->showMsg(NULL);
		return;
	}
	
	PtrList msgList;
	IcqDB::loadMsg(uin, msgList);

	IcqUser &myInfo = icqLink->myInfo;
	CString nick;
	if (uin) {
		IcqContact *c = icqLink->findContact(uin);
		if (c)
			nick = c->nick.c_str();
		else
			nick.Format("%lu", uin);
	}

	CListCtrl &ctrl = GetListCtrl();

	IcqMsg *msg = NULL;
	for (int i = 0; !msgList.empty(); ++i) {
		msg = (IcqMsg *) msgList.front();
		msgList.pop_front();

		CString str;
		if (msg->flags & MF_RECEIVED) {
			if (uin)
				str = nick;
			else
				str.Format("%lu", msg->uin);
		} else
			str = icqLink->myInfo.nick.c_str();
		ctrl.InsertItem(i, str);
		ctrl.SetItemData(i, (DWORD) msg);

		CTime t((time_t) msg->when);
		ctrl.SetItemText(i, 1, t.Format("%Y-%m-%d"));
		ctrl.SetItemText(i, 2, t.Format("%H:%M:%S"));

		getMsgText(msg, str);
		ctrl.SetItemText(i, 3, str);
	}

	msgView->showMsg(msg);
}

void CMsgListView::delSelectedMsg()
{
	CListCtrl &ctrl = GetListCtrl();
	POSITION pos;
	while ((pos = ctrl.GetFirstSelectedItemPosition()) != NULL) {
		int i = ctrl.GetNextSelectedItem(pos);
		IcqMsg *msg = (IcqMsg *) ctrl.GetItemData(i);
		uint32 uin = (msg->isSysMsg() ? 0 : msg->uin);

		IcqDB::delMsg(uin, i);
		ctrl.DeleteItem(i);
		delete msg;
	}
}

void CMsgListView::exportTxt(CStdioFile &file)
{
	CString nick;
	
	CListCtrl &ctrl = GetListCtrl();
	POSITION pos = ctrl.GetFirstSelectedItemPosition();
	while (pos) {
		int i = ctrl.GetNextSelectedItem(pos);
		IcqMsg *msg = (IcqMsg *) ctrl.GetItemData(i);

		if (nick.IsEmpty()) {
			IcqContact *c = icqLink->findContact(msg->uin);
			if (c && !msg->isSysMsg())
				nick = c->nick.c_str();
			else
				nick.Format("%lu", c->uin);
		}
		exportMsgTxt(file, msg, nick);
		delete msg;
	}
}

BEGIN_MESSAGE_MAP(CMsgListView, CListView)
	//{{AFX_MSG_MAP(CMsgListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DEL, OnUpdateEditDel)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgListView diagnostics

#ifdef _DEBUG
void CMsgListView::AssertValid() const
{
	CListView::AssertValid();
}

void CMsgListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMsgListView message handlers

int CMsgListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CListCtrl &ctrl = GetListCtrl();

	ctrl.SetExtendedStyle(ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CString str;
	str.LoadString(IDS_MSG_SENDER);
	ctrl.InsertColumn(0, str, LVCFMT_LEFT, 100);
	str.LoadString(IDS_DATE);
	ctrl.InsertColumn(1, str, LVCFMT_LEFT, 100);
	str.LoadString(IDS_TIME);
	ctrl.InsertColumn(2, str, LVCFMT_LEFT, 100);
	str.LoadString(IDS_CONTENT);
	ctrl.InsertColumn(3, str, LVCFMT_LEFT, 500);
	
	return 0;
}

BOOL CMsgListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS;
	return CListView::PreCreateWindow(cs);
}

void CMsgListView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl &ctrl = GetListCtrl();
	IcqMsg *msg = NULL;
	POSITION pos = ctrl.GetFirstSelectedItemPosition();
	if (pos) {
		int i = ctrl.GetNextSelectedItem(pos);
		msg = (IcqMsg *) ctrl.GetItemData(i);
	}
	msgView->showMsg(msg);
	
	*pResult = 0;
}

void CMsgListView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMenu menu;
	menu.LoadMenu(IDR_MSGMGR_POPUP);
	CPoint pt;
	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, GetParentFrame());
	
	*pResult = 0;
}

void CMsgListView::OnUpdateEditDel(CCmdUI* pCmdUI) 
{
	if (GetParentFrame()->GetActiveView() == this)
		pCmdUI->Enable(GetListCtrl().GetSelectedCount() > 0);
}

void CMsgListView::OnDestroy() 
{
	clear();

	CListView::OnDestroy();
}
