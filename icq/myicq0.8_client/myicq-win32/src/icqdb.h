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
#include <db.h>

#define MAX_BLOCK_SIZE	4096

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
	DBOutStream &operator <<(const string &str) {
		return operator <<(str.c_str());
	}
	DBOutStream &operator <<(StrList &strList);
	
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

class IcqDB {
public:
	static void setDir(const char *dir) {
		dbDir = dir;
	}

	static bool saveMsg(IcqMsg &msg);
	static bool loadMsg(uint32 uin, PtrList &msgList);
	static bool delMsg(uint32 uin);
	static bool delMsg(uint32 uin, int item);
	static bool saveContact(IcqContact &c);
	static bool loadContact(IcqContact &c);
	static bool loadContact(PtrList &contactList);
	static bool delContact(uint32 uin);
	static bool saveUser(IcqUser &user);
	static bool loadUser(IcqUser &user);
	static bool saveOptions(IcqOption &options);
	static bool loadOptions(IcqOption &options);
	static bool saveGroupInfo(DBSerialize &obj);
	static bool loadGroupInfo(DBSerialize &obj);

	static bool exportMsg(const char *pathName, uint32 uin);
	static bool importRecord(const char *pathName);
	static bool saveConfig(const char *fileName, DBSerialize &obj);
	static bool loadConfig(const char *fileName, DBSerialize &obj);
	static bool getMsgUinList(UinList &uinList);

	static void loadQuickReply(StrList &l);
	static void saveQuickReply(StrList &l);
	static void loadAutoReply(StrList &l);
	static void saveAutoReply(StrList &l);

private:
	static DB *getDBFullPath(const char *pathName, bool dup = false);
	static DB *getDB(const char *fileName, bool dup = false) {
		return getDBFullPath((dbDir + fileName).c_str(), dup);
	}
	static bool saveBlock(const char *fileName, uint32 index, DBSerialize &obj, bool dup);
	static bool saveBlock(DB *db, uint32 index, DBSerialize &obj);
	static bool loadBlock(const char *fileName, uint32 index, DBSerialize &obj);
	static bool delIndex(const char *fileName, uint32 index);

	static void loadStrList(const char *file, StrList &l);
	static void saveStrList(const char *file, StrList &l);

	static string dbDir;
};

#endif
