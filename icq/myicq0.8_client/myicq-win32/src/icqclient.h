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

#ifndef _ICQ_CLIENT_H
#define _ICQ_CLIENT_H

#include "icqdb.h"
#include "contactinfo.h"

enum {
	LOGIN_SUCCESS,
	LOGIN_INVALID_UIN,
	LOGIN_WRONG_PASSWD,
};

enum {
	MSG_TEXT,
	MSG_AUTO_REPLY,
	MSG_AUTH_ACCEPTED,
	MSG_AUTH_REQUEST,
	MSG_AUTH_REJECTED,
	MSG_ADDED,
	MSG_TCP_REQUEST,
	MSG_TCP_ACCEPTED,
	MSG_TCP_REJECTED,
};

enum {
	ADD_FRIEND_ACCEPTED,
	ADD_FRIEND_AUTH_REQ,
	ADD_FRIEND_REJECTED,
};

enum {
	CONTACT_FRIEND,
	CONTACT_STRANGER,
	CONTACT_IGNORE
};

enum {
	CF_OPTION_INVISIBLE,
	CF_OPTION_POPUP_ONLINE,
	CF_OPTION_GREETING,
	CF_OPTION_POPUP_OFFLINE,
	CF_OPTION_IGNORE,
	CF_CUSTOMSOUND,
	CF_DIALOGMODE,
	CF_SENDENTER,
	
	NR_CONTACT_FLAGS
};

enum {
	UF_ALWAYS_ON_TOP,
	UF_TASKBAR_STYLE,
	UF_ONLINE_NOTIFY,
	UF_MONITOR_NETWORK,
	UF_COLORFUL_URL,
	UF_TASKBAR_ICON,
	UF_SHOW_ONLINE,
	UF_AUTO_POPUP_MSG,
	UF_IGNORE_STRANGER,
	UF_LOGIN_INVIS,
	UF_USE_HOTKEY,
	UF_DEFAULT_HOTKEY,
	UF_USE_PROXY,
	UF_SOUND_ON,
	UF_AUTO_REPLY,
	UF_AUTO_SWITCH_STATUS,
	UF_AUTO_CANCEL_AWAY,

	UF_STORE_PASSWD,
	
	NR_USER_FLAGS
};

enum {
	SOUND_ONLINE,
	SOUND_MSG,

	SOUND_SYSMSG,
	SOUND_FOLDER,

	NR_SOUNDS
};

enum {
	PROXY_SOCKS,
	PROXY_HTTP,

	NR_PROXY_TYPES
};

#define MYICQ_TCP_VER		1
#define NR_CUSTOM_SOUNDS	2
#define MF_RECEIVED			0x01
#define MF_RELAY			0x02


struct SEARCH_RESULT {
	uint32 uin;
	uint8 online;
	uint8 face;
	string nick;
	string province;
};

class IcqMsg : public DBSerialize {
public:
	bool isSysMsg();

	void save(DBOutStream &out);
	void load(DBInStream &in);

	uint8 type;
	uint8 flags;
	uint32 uin;
	uint32 when;
	string text;
};

class IcqInfo : public ContactInfo, public DBSerialize {
public:
	IcqInfo();

	void save(DBOutStream &out);
	void load(DBInStream &in);
};

class IcqContact : public IcqInfo {
public:
	IcqContact();
	~IcqContact();

	void load(DBInStream &in);
	void save(DBOutStream &out);

	int type;
	uint32 ip;
	uint16 port;
	uint32 realIP;

	bitset<NR_CONTACT_FLAGS> flags;
	string soundFiles[NR_CUSTOM_SOUNDS];
	string greeting;
	int winX, winY;
};

class IcqUser : public IcqInfo {
public:
	IcqUser();

	void load(DBInStream &in);
	void save(DBOutStream &out);

	string passwd;
	uint8 auth;
};

class ProxyInfo : public DBSerialize {
public:
	ProxyInfo() {
		port = 0;
		resolve = 0;
	}
	void load(DBInStream &in);
	void save(DBOutStream &out);

	string host;
	uint16 port;
	string username;
	string passwd;
	uint8 resolve;
};

class IcqOption : public DBSerialize {
public:
	IcqOption();
	~IcqOption();

	void load(DBInStream &in);
	void save(DBOutStream &out);
	void playSound(int sound, IcqContact *c = NULL);

	bitset<NR_USER_FLAGS> flags;
	uint32 hotKey;
	string soundFiles[NR_SOUNDS];
	string host;
	uint16 port;
	string autoReplyText;
	string quickReplyText;
	uint32 autoStatus;
	uint32 autoStatusTime;
	string skin;

	uint8 proxyType;
	ProxyInfo proxy[NR_PROXY_TYPES];

private:
	bool playSound(const char *file);
};


#define MAX_MSG_LEN		512

class TextOutStream {
public:
	TextOutStream() {
		cursor = text;
	}

	TextOutStream &operator <<(uint32 dw);
	TextOutStream &operator <<(const char *str);
	operator const char *() {
		return text;
	}

private:
	char text[MAX_MSG_LEN];
	char *cursor;
};

class TextInStream {
public:
	TextInStream(const char *s);
	
	TextInStream &operator >>(uint8 &b);
	TextInStream &operator >>(uint16 &w);
	TextInStream &operator >>(uint32 &dw);
	TextInStream &operator >>(string &s);

private:
	char text[MAX_MSG_LEN];
	char *cursor;
};


#endif
