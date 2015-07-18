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

#ifndef _REF_OBJECT_H
#define _REF_OBJECT_H


class RefObject {
public:
	RefObject() {
		refCount = 1;
	}

	int addRef() {
		return ++refCount;
	}
	int release() {
		register int ret = --refCount;
		if (!ret)
			delete this;
		return ret;
	}

protected:
	virtual ~RefObject() {}

	int refCount;
};


#endif
