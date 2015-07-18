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

#include "icqlist.h"

void IcqListItem::remove()
{
	prev->next = next;
	next->prev = prev;
	prev = next = this;
}

void IcqList::add(IcqListItem *item)
{
	item->prev = head.prev;
	item->next = &head;
	head.prev->next = item;
	head.prev = item;
}

void IcqList::addHead(IcqListItem *item)
{
	item->prev = &head;
	item->next = head.next;
	head.next->prev = item;
	head.next = item;
}
