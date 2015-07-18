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

// MsgMgrWnd.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "MsgMgrWnd.h"
#include "MsgView.h"
#include "MsgTreeView.h"
#include "MsgListView.h"
#include "MsgSearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MESSAGE_CFG		"message.cfg"

/////////////////////////////////////////////////////////////////////////////
// CMsgMgrWnd

CMsgMgrWnd::CMsgMgrWnd()
{
	treeView = NULL;
	listView = NULL;
	msgView = NULL;
}

CMsgMgrWnd::~CMsgMgrWnd()
{
}

void CMsgMgrWnd::save(DBOutStream &out)
{
	treeView->save(out);
	listView->save(out);
	msgView->save(out);
}

void CMsgMgrWnd::load(DBInStream &in)
{
	treeView->load(in);
	listView->load(in);
	msgView->load(in);
}

BEGIN_MESSAGE_MAP(CMsgMgrWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CMsgMgrWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_DEL, OnEditDel)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_EXPORT_TXT, OnExportTxt)
	ON_COMMAND(ID_VIEW_BGCOLOR, OnViewBgcolor)
	ON_COMMAND(ID_VIEW_FONT, OnViewFont)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_CONTACT, OnUpdateExportContact)
	ON_COMMAND(ID_EXPORT_CONTACT, OnExportContact)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DEL, OnUpdateEditDel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONT, OnUpdateViewFont)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_BAK, OnUpdateExportBak)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_SEARCH, OnEditSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgMgrWnd message handlers

int CMsgMgrWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
/*
	if (!toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!toolBar.LoadToolBar(IDR_MSGMGR))
		return -1;
*/
	treeView = (CMsgTreeView *) splitter.GetPane(0, 0);
	listView = (CMsgListView *) splitterRight.GetPane(0, 0);
	msgView = (CMsgView *) splitterRight.GetPane(1, 0);

	treeView->setListView(listView);
	listView->setMsgView(msgView);

	IcqDB::loadConfig(MESSAGE_CFG, *this);

	SetActiveView(treeView);

	return 0;
}

BOOL CMsgMgrWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (!splitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!splitter.CreateView(0, 0, RUNTIME_CLASS(CMsgTreeView), CSize(200, 0), pContext))
		return FALSE;

	if (!splitterRight.CreateStatic(&splitter, 2, 1, WS_CHILD | WS_VISIBLE,
		splitter.IdFromRowCol(0, 1)))
		return FALSE;

	if (!splitterRight.CreateView(0, 0, RUNTIME_CLASS(CMsgListView), CSize(0, 400), pContext))
		return FALSE;

	if (!splitterRight.CreateView(1, 0, RUNTIME_CLASS(CMsgView), CSize(0, 0), pContext))
		return FALSE;
	
	return TRUE;
}

void CMsgMgrWnd::ActivateFrame(QID *qid, int nCmdShow)
{
	treeView->expand(qid);

	CFrameWnd::ActivateFrame(SW_MAXIMIZE);
}

void CMsgMgrWnd::OnEditDel() 
{
	if (AfxMessageBox(IDS_PROMPT_DEL_MSG, MB_YESNO) != IDYES)
		return;
	
	if (GetFocus() == listView)
		listView->delSelectedMsg();
	else
		treeView->delSelectedMsg();
}

void CMsgMgrWnd::OnFileExit() 
{
	DestroyWindow();
}

void CMsgMgrWnd::OnExportTxt() 
{
	CString fileName;
	QID *qid = treeView->getQID();

	if (qid)
		fileName = qid->toString();

	CString filter;
	filter.LoadString(IDS_FILTER_TXT);
	CFileDialog dlg(FALSE, "txt", fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (dlg.DoModal() != IDOK)
		return;

	CStdioFile file(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite);
	CWnd *focus = GetFocus();
	if (focus == treeView)
		treeView->exportTxt(file);
	else if (focus == listView)
		listView->exportTxt(file);
}

void CMsgMgrWnd::OnViewBgcolor() 
{
	CColorDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	COLORREF color = dlg.GetColor();
	CWnd *focus = GetFocus();
	if (focus == treeView)
		treeView->GetTreeCtrl().SetBkColor(color);
	else if (focus == listView) {
		listView->GetListCtrl().SetBkColor(color);
		listView->GetListCtrl().SetTextBkColor(color);
		listView->Invalidate();
	} else if (focus == &msgView->m_msgEdit)
		msgView->m_msgEdit.SetBackgroundColor(FALSE, color);
}

void CMsgMgrWnd::OnViewFont() 
{
	CWnd *focus = GetFocus();

	if (focus == treeView || focus == listView) {
		CColorDialog dlg;
		if (dlg.DoModal() != IDOK)
			return;

		COLORREF color = dlg.GetColor();
		if (focus == treeView)
			treeView->GetTreeCtrl().SetTextColor(color);
		else if (focus == listView) {
			listView->GetListCtrl().SetTextColor(color);
			listView->Invalidate();
		}
	} else if (focus == &msgView->m_msgEdit) {
		CHARFORMAT cf;
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_CHARSET;
		msgView->m_msgEdit.GetDefaultCharFormat(cf);

		LOGFONT lf;
		ZeroMemory(&lf, sizeof(lf));
		lstrcpy(lf.lfFaceName, cf.szFaceName);
		lf.lfCharSet = cf.bCharSet;
		CDC *pDC = GetDC();
		lf.lfHeight = -MulDiv(cf.yHeight, pDC->GetDeviceCaps(LOGPIXELSY), 72) / 20;
		ReleaseDC(pDC);

		CFontDialog dlg(&lf);
		dlg.m_cf.rgbColors = cf.crTextColor;
		if (dlg.DoModal() != IDOK)
			return;

		cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE;
		lstrcpy(cf.szFaceName, dlg.GetFaceName());
		cf.yHeight = dlg.GetSize() * 2;
		cf.crTextColor = dlg.GetColor();
		cf.dwEffects = 0;

		msgView->m_msgEdit.SetDefaultCharFormat(cf);
		msgView->Invalidate();
	}
}

void CMsgMgrWnd::OnUpdateExportContact(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}

void CMsgMgrWnd::OnExportContact() 
{
	CString fileName;
	QID *qid = treeView->getQID();
	if (qid)
		fileName = qid->toString();

	CString filter;
	filter.LoadString(IDS_FILTER_TXT);
	CFileDialog dlg(FALSE, "txt", fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (dlg.DoModal() != IDOK)
		return;

	CStdioFile file(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite);
	if (GetActiveView() == treeView)
		treeView->exportContact(file);
}

void CMsgMgrWnd::OnUpdateEditDel(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}

void CMsgMgrWnd::OnUpdateViewFont(CCmdUI* pCmdUI) 
{
	CMenu *menu = pCmdUI->m_pMenu;
	if (menu) {
		CString str;
		str.LoadString(IDS_VIEW_FONTCOLOR);
		menu->ModifyMenu(pCmdUI->m_nID, 0, pCmdUI->m_nID, str);
	}
}

void CMsgMgrWnd::OnUpdateExportBak(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}

void CMsgMgrWnd::OnDestroy() 
{
	IcqDB::saveConfig(MESSAGE_CFG, *this);

	CFrameWnd::OnDestroy();	
}

void CMsgMgrWnd::OnEditSearch() 
{
	new CMsgSearchDlg(this);
}
