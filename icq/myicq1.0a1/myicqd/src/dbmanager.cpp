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

#include "icqsocket.h"
#include "dbmanager.h"
#include "refobject.h"
#include "queue.h"

char *conv10(uint32 num, char *bufEnd)
{
	do {
		*--bufEnd = num % 10 + '0';
		num /= 10;
	} while (num);

	return bufEnd;
}


IMPLEMENT_SLAB(DBRequest, 16)


DBRequest::DBRequest(uint8 f, DB_CALLBACK cb, RefObject *obj, uint32 d)
{
	flags = f;
	callback = cb;
	refObj = obj;
	data = d;
	res = NULL;

	cursor = sql;
}


MYSQL *mysqlWrite;

static Queue	queryQueue;
static Queue	updateQueue;
static Queue	resultQueue;


static MYSQL *mysqlInit(DB_INFO &info)
{
	MYSQL *mysql = mysql_init(NULL);
	if (!mysql)
		return NULL;

	if (!mysql_real_connect(mysql,
		info.host.c_str(),
		info.user.c_str(),
		info.passwd.c_str(),
		info.db.c_str(),
		info.port, NULL, 0)) {
		mysql_close(mysql);
		return NULL;
	}
	return mysql;
}

static void mysqlClose(MYSQL *mysql)
{
	if (mysql)
		mysql_close(mysql);
}

inline DBRequest *getDBRequest(Queue &q)
{
	ListHead *pos = q.get();
	return LIST_ENTRY(pos, DBRequest, listItem);
}

inline void putDBRequest(DBRequest *req)
{
	if (req->callback)
		resultQueue.put(&req->listItem);
	else
		delete req;
}

bool DBManager::init(DB_INFO &dbMaster)
{
	mysqlWrite = mysqlInit(dbMaster);
	if (!mysqlWrite)
		return false;
	return true;
}

void DBManager::destroy()
{
	mysqlClose(mysqlWrite);
}

void DBManager::dispatch()
{
	while (!resultQueue.isEmpty()) {
		ListHead *pos = resultQueue.get();
		DBRequest *req = LIST_ENTRY(pos, DBRequest, listItem);
		RefObject *obj = req->refObj;
		MYSQL_RES *res = req->res;

		req->callback(req);
		if (obj)
			obj->release();

		if (!(req->flags & DBF_UPDATE) && res)
			mysql_free_result(res);
		delete req;
	}
}

void DBManager::query(DBRequest *req)
{
	if (req->callback && req->refObj)
		req->refObj->addRef();

	Queue &q = ((req->flags & DBF_UPDATE) ? updateQueue : queryQueue);
	q.put(&req->listItem);
}

DBManager::DBManager()
{
	mysqlRead = NULL;
}

DBManager::~DBManager()
{
	mysqlClose(mysqlRead);
}

bool DBManager::create(DB_INFO &dbSlave)
{
	mysqlRead = mysqlInit(dbSlave);
	if (!mysqlRead)
		return false;
	return true;
}

void DBManager::processQuery()
{
	for (;;) {

		DBRequest *req = getDBRequest(queryQueue);

		if (mysql_real_query(mysqlRead, req->sql, req->sqlLen()) == 0)
			req->res = mysql_store_result(mysqlRead);
		putDBRequest(req);
	}
}

void DBManager::processUpdate()
{
	for (;;) {
		DBRequest *req = getDBRequest(updateQueue);
		int r = mysql_real_query(mysqlWrite, req->sql, req->sqlLen());
		if (!r && (req->flags & DBF_INSERT))
			req->lastInsertID = (uint32) mysql_insert_id(mysqlWrite);

		req->ret = r;
		putDBRequest(req);
	}
}
