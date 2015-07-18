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

// AnimPic.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "AnimPic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_ANIMATE	1001

/////////////////////////////////////////////////////////////////////////////
// CAnimPic

CAnimPic::CAnimPic()
{
	frame = 0;
}

CAnimPic::~CAnimPic()
{
}


void CAnimPic::start()
{
	SetTimer(IDT_ANIMATE, 400, NULL);
	OnTimer(IDT_ANIMATE);
}

void CAnimPic::stop()
{
	KillTimer(IDT_ANIMATE);
}


BEGIN_MESSAGE_MAP(CAnimPic, CStatic)
	//{{AFX_MSG_MAP(CAnimPic)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimPic message handlers

void CAnimPic::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_ANIMATE) {
		CImageList &imageList = getApp()->largeImageList;
		SetIcon(imageList.ExtractIcon(frame));
		if (++frame >= getApp()->nrFaces)
			frame = 0;
	} else
		CStatic::OnTimer(nIDEvent);
}
