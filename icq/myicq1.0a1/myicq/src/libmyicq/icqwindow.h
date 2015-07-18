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

#ifndef _ICQ_WINDOW_H
#define _ICQ_WINDOW_H

#include "icqtypes.h"

enum {
	WIN_SEND_MESSAGE,
	WIN_SEND_REQUEST,
	WIN_VIEW_DETAIL,
	WIN_MODIFY_DETAIL,
	WIN_SEARCH_WIZARD,
	WIN_REG_WIZARD,
	WIN_SYS_MESSAGE,
	WIN_VIEW_MESSAGE,
	WIN_SYS_HISTORY,
	WIN_ADD_FRIEND,
	WIN_DEL_FRIEND,
	WIN_GROUP_CREATE_WIZARD,
	WIN_GROUP_SEARCH_WIZARD,
};

class IcqWindow {
public:
	IcqWindow(int type, QID *qid = NULL);
	virtual ~IcqWindow();

	virtual bool isSeq(uint32 seq) {
		return (this->seq == seq);
	}
	virtual void onAck(uint32 seq) {}
	virtual void onSendError(uint32 seq) {}

	int type;
	QID qid;
	uint32 seq;
};

class GroupWindow {
public:
	GroupWindow(uint32 id);
	virtual ~GroupWindow();

	virtual void onAck() {}
	virtual void onSendError() {}

	uint32 id;
	uint32 seq;
};

#endif
