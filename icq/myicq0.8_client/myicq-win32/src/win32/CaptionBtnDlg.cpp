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

// CaptionBtnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaptionBtnDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptionBtnDlg dialog


CCaptionBtnDlg::CCaptionBtnDlg(int buttons, UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CCaptionBtnDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	nrButtons = buttons;
	pressedButton = -1;
	inButton = FALSE;
}

int CCaptionBtnDlg::hitTest(CPoint &pt)
{
	int w = nrButtons * 14;
	CRect rc;
	GetWindowRect(rc);
	rc.right -= 20;
	rc.left = rc.right - w;
	rc.top += 6;
	rc.bottom = rc.top + 12;
	if (rc.PtInRect(pt))
		return (rc.right - pt.x - 1) / 14;
	return -1;
}

void CCaptionBtnDlg::drawButton(int button, BOOL pressed)
{
	CWindowDC dc(this);
	CRect rc;
	GetWindowRect(rc);
	rc.OffsetRect(-rc.left, -rc.top);

	rc.right -= 20 + button * 14;
	rc.left = rc.right - 12;
	rc.top += 6;
	rc.bottom = rc.top + 12;
	CRgn rgn;
	rgn.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	dc.SelectClipRgn(&rgn);

	drawCaptionBtn(&dc, rc, button, pressed);
}

void CCaptionBtnDlg::drawButtons()
{
	for (int i = 0; i < nrButtons; i++)
		drawButton(i, FALSE);
}

void CCaptionBtnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaptionBtnDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaptionBtnDlg, CDialog)
	//{{AFX_MSG_MAP(CCaptionBtnDlg)
	ON_WM_NCPAINT()
	ON_WM_ACTIVATE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCACTIVATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaptionBtnDlg message handlers

void CCaptionBtnDlg::OnNcPaint() 
{
	CDialog::OnNcPaint();

	drawButtons();
}

void CCaptionBtnDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	drawButtons();
}

void CCaptionBtnDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	int button = hitTest(point);
	if (button >= 0) {
		SetCapture();
		pressedButton = button;
		inButton = TRUE;
		drawButton(button, TRUE);
	}
	CDialog::OnNcLButtonDown(nHitTest, point);
}

void CCaptionBtnDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetCapture() == this) {
		ReleaseCapture();
		ClientToScreen(&point);
		int button = hitTest(point);
		if (button >= 0 && pressedButton == button) {
			drawButton(button, FALSE);
			onCaptionBtnClicked(button);
		}
		pressedButton = -1;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CCaptionBtnDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetCapture() == this) {
		ClientToScreen(&point);

		int button = hitTest(point);
		if (inButton && button != pressedButton) {
			inButton = FALSE;
			drawButton(pressedButton, FALSE);
		} else if (!inButton && button == pressedButton) {
			inButton = TRUE;
			drawButton(pressedButton, TRUE);
		}
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CCaptionBtnDlg::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT res = Default();
	drawButtons();
	return res;
}

BOOL CCaptionBtnDlg::OnNcActivate(BOOL bActive) 
{
	BOOL ret = CDialog::OnNcActivate(bActive);
	drawButtons();
	return ret;
}
