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

// AnimFace.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "AnimFace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_ANIMATE		1

/////////////////////////////////////////////////////////////////////////////
// CAnimFace

CAnimFace::CAnimFace()
{
	frame = 0;
}

CAnimFace::~CAnimFace()
{
}

void CAnimFace::start()
{
	frame = 0;
	OnTimer(IDT_ANIMATE);
	SetTimer(IDT_ANIMATE, 400, NULL);
}

void CAnimFace::stop()
{
	KillTimer(IDT_ANIMATE);
}


BEGIN_MESSAGE_MAP(CAnimFace, CStatic)
	//{{AFX_MSG_MAP(CAnimFace)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimFace message handlers

void CAnimFace::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_ANIMATE) {
		SetIcon(getApp()->getLargeFace(frame));
		if (++frame >= NR_FACES)
			frame = 0;
	} else
		CStatic::OnTimer(nIDEvent);
}
