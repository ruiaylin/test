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

#include "queue.h"

ListHead *Queue::get()
{
	sem.down();

	mutex.lock();
	ListHead *item = list.removeHead();
	mutex.unlock();

	return item;
}

void Queue::put(ListHead *item)
{
	mutex.lock();
	list.add(item);
	mutex.unlock();

	sem.up();
}
