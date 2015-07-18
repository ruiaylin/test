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

#include "icqwindow.h"
#include "icqlink.h"


IcqWindow::IcqWindow(int type, QID *qid)
{
	this->type = type;
	if (qid)
		this->qid = *qid;
	seq = 0;
	icqLink->windowList.push_back(this);
}

IcqWindow::~IcqWindow()
{
	icqLink->windowList.remove(this);
}

GroupWindow::GroupWindow(uint32 id)
{
	this->id = id;
	seq = 0;
	icqLink->groupWindowList.push_back(this);
}

GroupWindow::~GroupWindow()
{
	icqLink->groupWindowList.remove(this);
}
