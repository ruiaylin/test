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

#ifndef _ICQ_SKIN_H
#define _ICQ_SKIN_H

enum {
	SKIN_BACK,
	SKIN_FOLDER,
	SKIN_FOLDER_LIGHT,
	SKIN_SCROLLUP,
	SKIN_SCROLLDOWN,
	SKIN_SCROLLUP_PRESSED,
	SKIN_SCROLLDOWN_PRESSED,

	NR_BITMAPS
};

class IcqSkin {
public:
	BOOL load(LPCTSTR skinDir);

	CString name;
	CBitmap bitmaps[NR_BITMAPS];
};

#endif