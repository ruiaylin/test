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

// AnimButton.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "AnimButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_ANIMATE	1001

/////////////////////////////////////////////////////////////////////////////
// CAnimButton

CAnimButton::CAnimButton()
{
	pImageList = NULL;
	imageCount = 0;
	frame = 0;
}

CAnimButton::~CAnimButton()
{
}

void CAnimButton::stop(int frame)
{
	this->frame = frame;
	OnTimer(IDT_ANIMATE);
	KillTimer(IDT_ANIMATE);
}

void CAnimButton::setImageList(CImageList *pImageList, int n)
{
	this->pImageList = pImageList;
	if (n == -1)
		n = pImageList->GetImageCount();
	imageCount = n;
	OnTimer(IDT_ANIMATE);
}

void CAnimButton::start()
{
	SetTimer(IDT_ANIMATE, 250, NULL);
	OnTimer(IDT_ANIMATE);
}

BEGIN_MESSAGE_MAP(CAnimButton, CButtonST)
	//{{AFX_MSG_MAP(CAnimButton)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimButton message handlers

void CAnimButton::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_ANIMATE) {
		SetIcon(pImageList->ExtractIcon(frame));
		if (++frame >= imageCount)
			frame = 0;
	} else
		CButtonST::OnTimer(nIDEvent);
}
