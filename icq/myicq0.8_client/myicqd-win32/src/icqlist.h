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

#ifndef _ICQ_LIST_H_
#define _ICQ_LIST_H_

#include "icqtypes.h"

#define LIST_ENTRY(ptr, type, member) \
	((type *) ((char *) (ptr) - (unsigned long) (&((type *) 0)->member)))

#define LIST_FOR_EACH(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

class IcqListItem {
public:
	IcqListItem() {
		prev = next = this;
	}
	void remove();

	IcqListItem *prev, *next;
};

class IcqList {
public:
	bool isEmpty() {
		return (head.next == &head);
	}
	IcqListItem *getHead() {
		return head.next;
	}
	void add(IcqListItem *item);
	void addHead(IcqListItem *item);

	IcqListItem head;
};

#endif
