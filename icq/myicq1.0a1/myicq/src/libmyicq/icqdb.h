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

#ifndef _ICQ_DB_H
#define _ICQ_DB_H

#include "icqtypes.h"

#define MAX_BLOCK_SIZE	4096

/*
 * Utility class that reads from or writes to a data block
 */
class DBOutStream {
public:
	DBOutStream() {
		cursor = data;
	}
	char *getData() {
		return data;
	}
	int getSize() {
		return (cursor - data);
	}

	DBOutStream &operator <<(uint8 b);
	DBOutStream &operator <<(uint16 w);
	DBOutStream &operator <<(uint32 dw);
	DBOutStream &operator <<(const char *str);
	DBOutStream &operator <<(StrList &strList);
	DBOutStream &operator <<(const string &str) {
		return operator <<(str.c_str());
	}
	DBOutStream &operator <<(const QID &qid) {
		return (*this << qid.uin << qid.domain);
	}

private:
	char data[MAX_BLOCK_SIZE];
	char *cursor;
};

class DBInStream {
public:
	DBInStream(void *d, int n) {
		cursor = data = (char *) d;
		datalen = n;
	}
	DBInStream &operator >>(uint8 &b);
	DBInStream &operator >>(uint16 &w);
	DBInStream &operator >>(uint32 &dw);
	DBInStream &operator >>(string &str);
	DBInStream &operator >>(StrList &strList);
	DBInStream &operator >>(QID &qid) {
		return (*this >> qid.uin >> qid.domain);
	}

private:
	char *data;
	char *cursor;
	int datalen;
};

class DBSerialize {
public:
	virtual void save(DBOutStream &out) = 0;
	virtual void load(DBInStream &in) = 0;
};

class IcqMsg;
class IcqContact;
class IcqUser;
class IcqOption;

/*
 * Do all the things related with the database.
 */
class IcqDB {
public:
	static void setDir(const char *dir);

	static bool saveMsg(IcqMsg &msg);
	static bool loadMsg(QID *qid, PtrList &msgList, uint32 n = -1);
	static bool delMsg(QID *qid);
	static bool delMsg(QID *qid, int item);
	static bool saveContact(IcqContact &c);
	static bool loadContact(IcqContact &c);
	static bool loadContact(PtrList &contactList);
	static bool delContact(QID &qid);
	static bool saveUser(IcqUser &user);
	static bool loadUser(IcqUser &user);
	static bool saveOptions(IcqOption &options);
	static bool loadOptions(IcqOption &options);
	static bool saveGroupInfo(DBSerialize &obj);
	static bool loadGroupInfo(DBSerialize &obj);

	static bool exportMsg(const char *pathName, QID &qid);
	static bool importRecord(const char *pathName);
	static bool saveConfig(const char *fileName, DBSerialize &obj);
	static bool loadConfig(const char *fileName, DBSerialize &obj);
	static bool getMsgQIDList(StrList &qidList);

	static void loadQuickReply(StrList &l);
	static void saveQuickReply(StrList &l);
	static void loadAutoReply(StrList &l);
	static void saveAutoReply(StrList &l);
};

#endif
