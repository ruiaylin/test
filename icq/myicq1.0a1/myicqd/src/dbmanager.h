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

#ifndef _DB_MANAGER_H
#define _DB_MANAGER_H

#include <mysql.h>
#include "myicq.h"
#include "list.h"
#include "slab.h"

#define WRITE_STR(req, str)		req->writeString(str, sizeof(str) - 1)

#define MAX_SQL		4096


extern MYSQL *mysqlWrite;

char *conv10(uint32 num, char *bufEnd);

class RefObject;
class DBRequest;

typedef void (*DB_CALLBACK)(DBRequest *req);


enum {
	DBF_UPDATE = 0x01,
	DBF_INSERT = 0x02,
};

class DBRequest {
friend class DBManager;
public:
	DBRequest(uint8 flags, DB_CALLBACK cb = NULL, RefObject *obj = NULL, uint32 d = 0);

	void writeString(const char *text, int n) {
		if (cursor - sql + n <= MAX_SQL) {
			memcpy(cursor, text, n);
			cursor += n;
		}
	}
	DBRequest &operator <<(ICQ_STR &str) {
		if (cursor - sql + (str.len << 1) < MAX_SQL - 2) {
			*cursor++ = '\'';
			cursor += mysql_real_escape_string(mysqlWrite, cursor, str.text, str.len);
			*cursor++ = '\'';
		}
		return *this;
	}
	DBRequest &operator <<(char c) {
		if (cursor - sql <= (int) (MAX_SQL - sizeof(c)))
			*cursor++ = c;
		return *this;
	}
	DBRequest &operator <<(uint8 num) {
		*this << (uint32) num;
		return *this;
	}
	DBRequest &operator <<(uint16 num) {
		*this <<(uint32) num;
		return *this;
	}
	DBRequest &operator <<(uint32 num) {
		char buf[16];
		char *p = conv10(num, buf + sizeof(buf));
		writeString(p, buf + sizeof(buf) - p);
		return *this;
	}

	ListHead listItem;

	DB_CALLBACK callback;
	RefObject *refObj;
	uint32 data;

	union {
		MYSQL_RES *res;
		int ret;
	};

	uint32 lastInsertID;

private:
	int sqlLen() {
		return cursor - sql;
	}

	uint8 flags;
	char sql[MAX_SQL];
	char *cursor;

	DECLARE_SLAB(DBRequest)
};


class DBManager {
public:
	DBManager();
	~DBManager();

	bool create(DB_INFO &dbSlave);
	void processQuery();

	static bool init(DB_INFO &dbMaster);
	static void destroy();
	static void query(DBRequest *req);
	static void processUpdate();
	static void dispatch();

private:
	MYSQL *mysqlRead;
};

#endif
