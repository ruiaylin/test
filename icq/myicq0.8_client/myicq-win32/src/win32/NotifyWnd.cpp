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

// NotifyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "NotifyWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ICON_OFFSET		2
#define IDT_MOVE		1001
#define IDT_WAIT		1002

CNotifyWnd *CNotifyWnd::notifyWnd = NULL;

/////////////////////////////////////////////////////////////////////////////
// CNotifyWnd

CNotifyWnd::CNotifyWnd(HICON icon, const char *text, DWORD t)
{
	if (notifyWnd)
		delete notifyWnd;
	notifyWnd = this;
	
	hIcon = icon;
	strText = text;
	clrBackground = GetSysColor(COLOR_INFOBK);
	clrText = GetSysColor(COLOR_INFOTEXT);
	wndWidth = 200;
	wndHeight = 50;
	waitTime = t;

	SystemParametersInfo(SPI_GETWORKAREA, 0, rcWorkArea, 0);

	CreateEx(WS_EX_TOOLWINDOW, AfxRegisterWndClass(0), NULL, WS_POPUP,
		rcWorkArea.right - wndWidth, rcWorkArea.bottom, wndWidth, 0,
		NULL, NULL);

	SetTimer(IDT_MOVE, 20, NULL);
}

CNotifyWnd::~CNotifyWnd()
{
	notifyWnd = NULL;
}


BEGIN_MESSAGE_MAP(CNotifyWnd, CWnd)
	//{{AFX_MSG_MAP(CNotifyWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNotifyWnd message handlers

void CNotifyWnd::PostNcDestroy() 
{
	delete this;
}

void CNotifyWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CPen pen(PS_SOLID, 1, clrText);
	CBrush br(clrBackground);
	CFont font;
	font.Attach((HFONT)GetStockObject(DEFAULT_GUI_FONT));

	CPen *penOld = dc.SelectObject(&pen);
	CBrush *brOld = dc.SelectObject(&br);
	CFont *fontOld = dc.SelectObject(&font);
	COLORREF oldTextColor = dc.SetTextColor(clrText);
	int oldBkMode = dc.SetBkMode(TRANSPARENT);

	CRect rc(0, 0, wndWidth, wndHeight);
	dc.Rectangle(rc);
	rc.InflateRect(-1, -1);

	dc.DrawIcon(ICON_OFFSET, (wndHeight - 32) / 2, hIcon);
	rc.left += 32 + (ICON_OFFSET << 1);
	dc.DrawText(strText, rc, DT_SINGLELINE | DT_VCENTER);

	dc.SetTextColor(oldTextColor);
	dc.SetBkMode(oldBkMode);
	dc.SelectObject(fontOld);
	dc.SelectObject(penOld);
	dc.SelectObject(brOld);

	// Do not call CWnd::OnPaint() for painting messages
}

void CNotifyWnd::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_MOVE) {
		CRect rc;
		GetWindowRect(rc);
		rc.top -= 2;
		if (rc.top >= rcWorkArea.bottom - wndHeight)
			SetWindowPos(&wndTopMost,
				rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
		else {
			KillTimer(nIDEvent);
			if (waitTime)
				SetTimer(IDT_WAIT, waitTime, NULL);
		}
	} else if (nIDEvent == IDT_WAIT)
		DestroyWindow();
	else
		CWnd::OnTimer(nIDEvent);
}

void CNotifyWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	DestroyWindow();
}
