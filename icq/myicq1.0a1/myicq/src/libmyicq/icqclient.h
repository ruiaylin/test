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
#include "proxy.h"

// Message types
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

// Contact types
enum {
	CONTACT_FRIEND,
	CONTACT_STRANGER,
	CONTACT_IGNORE
};

// Flags that are specific to each contact
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

// System option flags
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

// All kinds of sound effects
enum {
	SOUND_ONLINE,
	SOUND_MSG,

	SOUND_SYSMSG,
	SOUND_FOLDER,

	NR_SOUNDS
};

// Proxy types
enum {
	PROXY_SOCKS,
	PROXY_HTTP,

	NR_PROXY_TYPES
};

#define MYICQ_HOMEPAGE		"http://myicq.cosoft.org.cn"

// The client's TCP protocol version
#define MYICQ_TCP_VER		1

// Number of sound effects that can be customized to each contact
#define NR_CUSTOM_SOUNDS	2

#define MAX_MSG_LEN			450

// Message flags
#define MF_RECEIVED			0x01
#define MF_RELAY			0x02

// Message format flags
#define MFF_BOLD			0x01
#define MFF_ITALIC			0x02
#define MFF_UNDERLINE		0x04


struct SEARCH_RESULT {
	uint32 uin;
	uint8 online;
	uint8 face;
	string nick;
	string province;
};

struct SERVER_INFO {
	string domain;
	string desc;
	uint32 sessionCount;
};

struct GROUP_TYPE_INFO {
	string name;
	string displayName;
};

struct GROUP_INFO {
	uint32 id;
	string name;
	uint16 type;
	uint16 num;
};

class MsgFormat {
public:
	MsgFormat() {
		fontSize = 0;
		flags = 0;
	}

	string fontName;
	uint8 fontSize;
	uint32 fontColor;
	uint8 flags;
};

class TextOutStream;

class IcqMsg : public DBSerialize {
public:
	IcqMsg();

	virtual void save(DBOutStream &out);
	virtual void load(DBInStream &in);

	bool isSysMsg();
	void encode(TextOutStream &out);
	void decode(const char *text);

	uint32 id;
	uint8 type;
	uint8 flags;
	QID qid;
	uint32 when;
	string text;
	MsgFormat format;
};

class IcqInfo : public ContactInfo, public DBSerialize {
public:
	IcqInfo();
	virtual ~IcqInfo() {}

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
};

class IcqUser : public IcqInfo {
public:
	IcqUser();

	void load(DBInStream &in);
	void save(DBOutStream &out);

	string passwd;
	uint8 auth;
};

class IcqOption : public DBSerialize {
public:
	IcqOption();
	virtual ~IcqOption();

	void load(DBInStream &in);
	void save(DBOutStream &out);
	void playSound(int sound, IcqContact *c = NULL);

	bitset<NR_USER_FLAGS> flags;		// System option flags
	uint32 hotKey;
	string soundFiles[NR_SOUNDS];
	string host;
	uint16 port;
	string autoReplyText;
	string quickReplyText;
	uint32 autoStatus;
	uint32 autoStatusTime;
	string skin;

	// Proxy stuff
	uint8 proxyType;
	ProxyInfo proxy[NR_PROXY_TYPES];

private:
	bool playSound(const char *file);
};


#define MAX_TEXT_SIZE	1024

/*
 * Since there may be many fields encoded in the text field of IcqMsg,
 * this class is an utility to do this.
 */
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
	char text[MAX_TEXT_SIZE];
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
	char text[MAX_TEXT_SIZE];
	char *cursor;
};


#endif
