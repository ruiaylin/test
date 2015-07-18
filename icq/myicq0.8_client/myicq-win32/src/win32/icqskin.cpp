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

#include "stdafx.h"
#include "icqskin.h"

static const char *fileNames[NR_BITMAPS] = {
	"back",
	"folder",
	"folder_light",
	"scrollup",
	"scrolldown",
	"scrollup_pressed",
	"scrolldown_pressed"
};


BOOL IcqSkin::load(LPCTSTR skinDir)
{
	CString dir(skinDir);
	int i = dir.ReverseFind('\\');
	name = dir.Mid(i + 1);

	dir += '\\';
	for (i = 0; i < NR_BITMAPS; ++i) {
		HBITMAP bm = (HBITMAP) LoadImage(AfxGetInstanceHandle(),
			dir + fileNames[i] + ".bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (!bm)
			return FALSE;

		bitmaps[i].DeleteObject();
		bitmaps[i].Attach(bm);
	}
	return TRUE;
}