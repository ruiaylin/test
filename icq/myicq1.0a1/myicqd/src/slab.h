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

#ifndef _SLAB_H
#define _SLAB_H

#include "list.h"
#include <stdio.h>

struct SLAB;

struct OBJ {
	OBJ *next;
	SLAB *slab;
};

struct SLAB {
	ListHead item;
	int inuse;
	OBJ *free;
};

class Cache {
public:
	Cache(int size, int n);
	~Cache();

	void *allocObj();
	void freeObj(void *p);

	static int reclaimAll();

private:
	SLAB *newSlab();
	int reclaim(int n = 0xffff);

	Cache *nextCache;
	ListHead slabList;
	ListHead *firstNotFull;

	int objSize;
	int numObjs;
	int numFreeSlabs;
	int slabSize;

	static Cache *cacheList;
};


#define DECLARE_SLAB(type)		\
private:	\
	static Cache type##_cache;	\
public:	\
	void *operator new(size_t) {	\
		return type##_cache.allocObj();	\
	}	\
	void operator delete(void *p) {	\
		type##_cache.freeObj(p);	\
	}

#define IMPLEMENT_SLAB(type, num)	\
	Cache type::type##_cache(sizeof(type), num);


#endif
