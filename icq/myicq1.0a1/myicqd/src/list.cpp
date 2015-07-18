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

#include "list.h"

void ListHead::remove()
{
	prev->next = next;
	next->prev = prev;
	prev = next = this;
}

ListHead *ListHead::removeHead()
{
	ListHead *t = next;
	next->remove();
	return t;
}

void ListHead::add(ListHead *item)
{
	item->prev = prev;
	item->next = this;
	prev->next = item;
	prev = item;
}

void ListHead::addHead(ListHead *item)
{
	item->prev = this;
	item->next = next;
	next->prev = item;
	next = item;
}
