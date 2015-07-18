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

// MsgEdit.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "MsgEdit.h"
#include "FORMATBA.H"
#include "COLORLIS.H"
#include "icqclient.h"
#include "ImageSelector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgEdit

CMsgEdit::CMsgEdit()
{
	formatBar = NULL;
}

CMsgEdit::~CMsgEdit()
{
}

void CMsgEdit::getMsgFormat(MsgFormat &f)
{
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(cf));
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_FACE | CFM_SIZE | CFM_COLOR;
	GetSelectionCharFormat(cf);

	f.fontName = cf.szFaceName;
	f.fontSize = (cf.yHeight + 5) / 20;
	f.fontColor = cf.crTextColor;

	uint8 flags = 0;
	if (cf.dwEffects & CFE_BOLD)
		flags |= MFF_BOLD;
	if (cf.dwEffects & CFE_ITALIC)
		flags |= MFF_ITALIC;
	if (cf.dwEffects & CFE_UNDERLINE)
		flags |= MFF_UNDERLINE;
	f.flags = flags;
}

BEGIN_MESSAGE_MAP(CMsgEdit, CRichEditCtrlEx)
	//{{AFX_MSG_MAP(CMsgEdit)
	ON_COMMAND(ID_CHAR_BOLD, OnCharBold)
	ON_COMMAND(ID_CHAR_ITALIC, OnCharItalic)
	ON_COMMAND(ID_CHAR_UNDERLINE, OnCharUnderline)
	ON_COMMAND(ID_CHAR_COLOR, OnCharColor)
	ON_COMMAND(ID_CHAR_EMOTION, OnCharEmotion)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_VIEW_FONT, OnViewFont)
	//}}AFX_MSG_MAP
	ON_NOTIFY(FN_GETFORMAT, AFX_IDW_TOOLBAR, OnGetCharFormat)
	ON_NOTIFY(FN_SETFORMAT, AFX_IDW_TOOLBAR, OnSetCharFormat)
	ON_NOTIFY(NM_RETURN, AFX_IDW_TOOLBAR, OnBarReturn)
	ON_COMMAND_RANGE(ID_COLOR0, ID_COLOR16, OnColorPick)
	ON_COMMAND_RANGE(ID_IMAGE0, ID_IMAGE0 + NR_EMOTIONS, OnEmotionPick)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_NOTIFY_REFLECT(EN_MSGFILTER, OnRclick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgEdit message handlers

void CMsgEdit::OnGetCharFormat(NMHDR* pNMHDR, LRESULT* pRes)
{
	ASSERT(pNMHDR != NULL);
	ASSERT(pRes != NULL);

	GetSelectionCharFormat(((CHARHDR*)pNMHDR)->cf);

	*pRes = 1;
}

void CMsgEdit::OnSetCharFormat(NMHDR* pNMHDR, LRESULT* pRes)
{
	ASSERT(pNMHDR != NULL);
	ASSERT(pRes != NULL);
	SetDefaultCharFormat(((CHARHDR*)pNMHDR)->cf);
	*pRes = 1;
}

void CMsgEdit::OnBarReturn(NMHDR*, LRESULT* )
{
	SetFocus();
}

void CMsgEdit::OnCharBold() 
{
	CHARFORMAT cf;
	cf.dwMask = CFM_BOLD;
	cf.dwEffects = (formatBar->GetToolBarCtrl().IsButtonChecked(ID_CHAR_BOLD) ? CFE_BOLD : 0);
	SetDefaultCharFormat(cf);
}

void CMsgEdit::OnCharItalic() 
{
	CHARFORMAT cf;
	cf.dwMask = CFM_ITALIC;
	cf.dwEffects = (formatBar->GetToolBarCtrl().IsButtonChecked(ID_CHAR_ITALIC) ? CFE_ITALIC : 0);
	SetDefaultCharFormat(cf);
}

void CMsgEdit::OnCharUnderline() 
{
	CHARFORMAT cf;
	cf.dwMask = CFM_UNDERLINE;
	cf.dwEffects = (formatBar->GetToolBarCtrl().IsButtonChecked(ID_CHAR_UNDERLINE) ? CFE_UNDERLINE : 0);
	SetDefaultCharFormat(cf);	
}

void CMsgEdit::OnColorPick(UINT nID)
{
	CHARFORMAT cf;
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = CColorMenu::GetColor(nID);
	cf.dwEffects = 0;
	SetDefaultCharFormat(cf);
}

void CMsgEdit::OnCharColor() 
{
	CColorMenu colorMenu;
	CRect rc;
	int index = formatBar->CommandToIndex(ID_CHAR_COLOR);
	formatBar->GetItemRect(index, &rc);
	formatBar->ClientToScreen(rc);
	colorMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,rc.left,rc.bottom, this);
}

void CMsgEdit::OnCharEmotion() 
{
	CRect rc;
	int index = formatBar->CommandToIndex(ID_CHAR_EMOTION);
	formatBar->GetItemRect(index, &rc);
	formatBar->ClientToScreen(rc);

	new ImageSelector(&getApp()->emotionImageList, emotions, rc.left, rc.bottom, 10, this);
}

void CMsgEdit::OnEmotionPick(UINT nID)
{
	int i = nID - ID_IMAGE0;
	insertEmotion(i);
}

void CMsgEdit::OnChange()
{
	char buf[4096];
	long line = LineFromChar(-1);
	int pos = LineIndex(-1);
	GetLine(line, buf, sizeof(buf));
	long start, end;
	GetSel(start, end);

	int i = start - pos - 1;
	char c = '\0';
	while (--i >= 0) {
		c = buf[i];
		if (c == '/' || c == ' ')
			break;
	}

	int k = -1;
	if (c == '/')
		k = findEmotion(buf + i);
	if (k < 0)
		return;

	pos += i;
	SetSel(pos, pos + strlen(emotions[k]));
	insertEmotion(k);
}

void CMsgEdit::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	MSGFILTER *pMsgFilter = (MSGFILTER *) pNMHDR;
	if (pMsgFilter->msg == WM_RBUTTONDOWN) {
		CMenu menu;
		menu.LoadMenu(IDR_MSGEDIT_POPUP);

		CHARRANGE cr;
		GetSel(cr);
		if (cr.cpMin == cr.cpMax) {
			menu.EnableMenuItem(ID_EDIT_CUT, MF_GRAYED);
			menu.EnableMenuItem(ID_EDIT_COPY, MF_GRAYED);
		}
		if (!CanPaste(CF_TEXT))
			menu.EnableMenuItem(ID_EDIT_PASTE, MF_GRAYED);

		CPoint pt;
		GetCursorPos(&pt);
		menu.GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, this);
	}
	
	*pResult = 0;
}

void CMsgEdit::OnEditCut() 
{
	Cut();
}

void CMsgEdit::OnEditCopy() 
{
	Copy();
}

void CMsgEdit::OnEditPaste() 
{
	Paste();
}

void CMsgEdit::OnViewFont() 
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_COLOR | CFM_FACE | CFM_SIZE;
	GetDefaultCharFormat(cf);

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	
	if (cf.dwEffects & CFE_BOLD)
		lf.lfWeight = FW_BOLD;
	if (cf.dwEffects & CFE_ITALIC)
		lf.lfItalic = TRUE;
	if (cf.dwEffects & CFE_UNDERLINE)
		lf.lfUnderline = TRUE;
	lstrcpy(lf.lfFaceName, cf.szFaceName);
	CDC *pDC = GetDC();
	lf.lfHeight = -MulDiv(cf.yHeight, pDC->GetDeviceCaps(LOGPIXELSY), 72) / 20;
	ReleaseDC(pDC);

	CFontDialog dlg(&lf);
	dlg.m_cf.rgbColors = cf.crTextColor;
	if (dlg.DoModal() != IDOK)
		return;

	lstrcpy(cf.szFaceName, dlg.GetFaceName());
	cf.yHeight = dlg.GetSize() * 2;
	cf.crTextColor = dlg.GetColor();
	cf.dwEffects = 0;
	if (dlg.IsBold())
		cf.dwEffects |= CFE_BOLD;
	if (dlg.IsItalic())
		cf.dwEffects |= CFE_ITALIC;
	if (dlg.IsUnderline())
		cf.dwEffects |= CFE_UNDERLINE;

	SetDefaultCharFormat(cf);

	formatBar->SyncToView();

	Invalidate();
}
