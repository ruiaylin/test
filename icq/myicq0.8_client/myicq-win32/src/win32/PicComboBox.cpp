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

// PicComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "PicComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicComboBox

CPicComboBox::CPicComboBox()
{
}

CPicComboBox::~CPicComboBox()
{
}

void CPicComboBox::addPics()
{
	SetImageList(&getApp()->largeImageList);

	COMBOBOXEXITEM cbi;
	ZeroMemory(&cbi, sizeof(cbi));
	int n = getApp()->nrFaces;
	for (int i = 0; i < n; i++) {
		cbi.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		cbi.iItem = i;
		cbi.iImage =
		cbi.iSelectedImage = i;
	
		InsertItem(&cbi);
	}
}

BEGIN_MESSAGE_MAP(CPicComboBox, CComboBoxEx)
	//{{AFX_MSG_MAP(CPicComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicComboBox message handlers
