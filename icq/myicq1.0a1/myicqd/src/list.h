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

#ifndef _LIST_H_
#define _LIST_H_

#include "icqtypes.h"

#define LIST_ENTRY(ptr, type, member) \
	((type *) ((char *) (ptr) - (unsigned long) (&((type *) 0)->member)))

#define LIST_FOR_EACH(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


class ListHead {
public:
	ListHead() {
		prev = next = this;
	}

	bool isEmpty() {
		return (next == this);
	}
	ListHead *removeHead();
	void add(ListHead *item);
	void addHead(ListHead *item);
	void remove();

	ListHead *prev, *next;
};


#endif
