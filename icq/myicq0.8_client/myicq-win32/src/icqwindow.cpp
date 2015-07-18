/***************************************************************************
                          icqwindow.cpp  -  description
                             -------------------
    begin                : Thu Apr 4 2002
    copyright            : (C) 2002 by Zhang Yong
    email                : z-yong163@163.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "icqwindow.h"
#include "icqlink.h"


IcqWindow::IcqWindow(int type, uint32 uin)
{
	this->type = type;
	this->uin = uin;
	seq = 0;
	icqLink->windowList.push_back(this);
}

IcqWindow::~IcqWindow()
{
	icqLink->windowList.remove(this);
}
