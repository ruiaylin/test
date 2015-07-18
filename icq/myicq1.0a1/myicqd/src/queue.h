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

#ifndef _QUEUE_H
#define _QUEUE_H

#include "list.h"
#include "sync.h"

class Queue {
public:
	bool isEmpty() {
		return list.isEmpty();
	}
	ListHead *get();
	void put(ListHead *item);

private:
	ListHead list;
	Mutex mutex;
	Semaphore sem;
};

#endif
