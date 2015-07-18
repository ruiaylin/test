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

// MsgView.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "MsgView.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgView

IMPLEMENT_DYNCREATE(CMsgView, CFormView)

CMsgView::CMsgView()
	: CFormView(CMsgView::IDD)
{
	//{{AFX_DATA_INIT(CMsgView)
	m_date = _T("");
	m_content = _T("");
	m_receiver = _T("");
	m_sender = _T("");
	m_time = _T("");
	//}}AFX_DATA_INIT
}

CMsgView::~CMsgView()
{
}

void CMsgView::save(DBOutStream &out)
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE;
	m_msgEdit.GetDefaultCharFormat(cf);

	out << cf.szFaceName << (uint32) cf.yHeight << cf.crTextColor;
}

void CMsgView::load(DBInStream &in)
{
	string faceName;
	uint32 size, color;
	in >> faceName >> size >> color;

	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE;
	lstrcpy(cf.szFaceName, faceName.c_str());
	cf.yHeight = size;
	cf.crTextColor = color;
	cf.dwEffects = 0;

	m_msgEdit.SetDefaultCharFormat(cf);
}

void CMsgView::showMsg(IcqMsg *msg)
{
	if (msg) {
		int sendFace, recvFace;
		IcqContact *c = icqLink->findContact(msg->uin);

		if (c) {
			m_sender = c->nick.c_str();
			sendFace = c->face;
		} else {
			m_sender.Format("%lu", msg->uin);
			sendFace = -1;
		}
		m_receiver = icqLink->myInfo.nick.c_str();
		recvFace = icqLink->myInfo.face;

		if (!(msg->flags & MF_RECEIVED)) {
			CString tmp = m_sender;
			m_sender = m_receiver;
			m_receiver = tmp;
			int t = sendFace;
			sendFace = recvFace;
			recvFace = t;
		}

		CTime time((time_t) msg->when);
		m_date = time.Format("%Y-%m-%d");
		m_time = time.Format("%H:%M:%S");
		getMsgText(msg, m_content);

		if (sendFace >= 0) {
			int i = getApp()->getImageIndex(sendFace);
			m_sendFace.SetIcon(getApp()->largeImageList.ExtractIcon(i));
			GetDlgItem(IDC_FACE_SENDER)->ShowWindow(SW_SHOW);
		} else
			GetDlgItem(IDC_FACE_SENDER)->ShowWindow(SW_HIDE);

		if (recvFace >= 0) {
			int i = getApp()->getImageIndex(recvFace);
			m_recvFace.SetIcon(getApp()->largeImageList.ExtractIcon(i));
			GetDlgItem(IDC_FACE_RECEIVER)->ShowWindow(SW_SHOW);
		} else
			GetDlgItem(IDC_FACE_RECEIVER)->ShowWindow(SW_HIDE);

	} else {
		GetDlgItem(IDC_FACE_RECEIVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FACE_SENDER)->ShowWindow(SW_HIDE);

		m_sender.Empty();
		m_receiver.Empty();
		m_date.Empty();
		m_time.Empty();
		m_content.Empty();
	}

	UpdateData(FALSE);
}

void CMsgView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgView)
	DDX_Control(pDX, IDC_MSG_EDIT, m_msgEdit);
	DDX_Control(pDX, IDC_FACE_SENDER, m_sendFace);
	DDX_Control(pDX, IDC_FACE_RECEIVER, m_recvFace);
	DDX_Text(pDX, IDC_DATE, m_date);
	DDX_Text(pDX, IDC_MSG_EDIT, m_content);
	DDX_Text(pDX, IDC_RECEIVER, m_receiver);
	DDX_Text(pDX, IDC_SENDER, m_sender);
	DDX_Text(pDX, IDC_TIME, m_time);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgView, CFormView)
	//{{AFX_MSG_MAP(CMsgView)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONT, OnUpdateViewFont)
	//}}AFX_MSG_MAP
	ON_NOTIFY(EN_MSGFILTER, IDC_MSG_EDIT, OnRclickMsgEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgView diagnostics

#ifdef _DEBUG
void CMsgView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMsgView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMsgView message handlers

void CMsgView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	CWnd *pWnd = GetDlgItem(IDC_MSG_EDIT);
	if (pWnd) {
		CRect rc;
		pWnd->GetWindowRect(rc);
		ScreenToClient(rc);
		pWnd->SetWindowPos(NULL, 0, 0, cx, cy - rc.top, SWP_NOMOVE | SWP_NOZORDER);
	}
}

void CMsgView::OnRclickMsgEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	MSGFILTER *pMsgFilter = (MSGFILTER *) pNMHDR;
	if (pMsgFilter->msg == WM_RBUTTONDOWN) {
		CMenu menu;
		menu.LoadMenu(IDR_MSGVIEW_POPUP);
		CPoint pt;
		GetCursorPos(&pt);
		menu.GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, GetTopLevelFrame());
	}
	
	*pResult = 0;
}

void CMsgView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	m_msgEdit.SetEventMask(m_msgEdit.GetEventMask() | ENM_MOUSEEVENTS);	
}

void CMsgView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	long start, end;
	m_msgEdit.GetSel(start, end);
	pCmdUI->Enable(start != end);
}

void CMsgView::OnEditSelectAll() 
{
	m_msgEdit.SetSel(0, -1);
}

void CMsgView::OnEditCopy() 
{
	CString str = m_msgEdit.GetSelText();
	
	if (OpenClipboard()) {
		HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, str.GetLength() + 1);
		char *p = (char *) GlobalLock(hMem);
		if (p) {
			lstrcpy(p, str);
			GlobalUnlock(hMem);
			SetClipboardData(CF_TEXT, hMem);
		}
		CloseClipboard();
	}
}

void CMsgView::OnUpdateViewFont(CCmdUI* pCmdUI) 
{
	CMenu *menu = pCmdUI->m_pMenu;
	if (menu && GetParentFrame()->GetActiveView() == this) {
		CString str;
		str.LoadString(IDS_VIEW_FONT);
		menu->ModifyMenu(pCmdUI->m_nID, 0, pCmdUI->m_nID, str);
	}	
}
