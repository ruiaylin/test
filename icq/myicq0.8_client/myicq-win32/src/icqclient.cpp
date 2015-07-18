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

#include "icqclient.h"
#include "icqdb.h"


bool IcqMsg::isSysMsg()
{
	return (uin < 1000 ||
			type == MSG_AUTH_ACCEPTED ||
			type == MSG_AUTH_REQUEST  ||
			type == MSG_AUTH_REJECTED ||
			type == MSG_ADDED);
}

void IcqMsg::save(DBOutStream &out)
{
	out << type << flags << uin << when << text;
}

void IcqMsg::load(DBInStream &in)
{
	in >> type >> flags >> uin >> when >> text;
}

IcqInfo::IcqInfo()
{
	uin = 0;
	face = 0;
	gender = 0;
	age = 0;
	blood = 0;

	status = STATUS_OFFLINE;
}

void IcqInfo::load(DBInStream &in)
{
	in >> uin >> face >> nick >> age >> gender >> country >> province >> city;
	in >> email >> address >> zipcode >> tel;
	in >> name >> blood >> college >> profession >> homepage >> intro;
}

void IcqInfo::save(DBOutStream &out)
{
	out << uin << face << nick << age << gender << country << province << city;
	out << email << address << zipcode << tel;
	out << name << blood << college << profession << homepage << intro;
}


IcqContact::IcqContact()
{
	type = CONTACT_FRIEND;
	ip = 
	realIP = 0;
	port = 0;
	winX = winY = -1;
}

IcqContact::~IcqContact()
{
}

void IcqContact::load(DBInStream &in)
{
	IcqInfo::load(in);

	uint32 f;
	in >> f;
	flags = bitset<NR_CONTACT_FLAGS>(f);

	in >> greeting;
	for (int i = 0; i < NR_CUSTOM_SOUNDS; i++)
		in >> soundFiles[i];
}

void IcqContact::save(DBOutStream &out)
{
	IcqInfo::save(out);
	
	out << flags.to_ulong();
	out << greeting;
	for (int i = 0; i < NR_CUSTOM_SOUNDS; i++)
		out << soundFiles[i];
}


IcqUser::IcqUser()
{
	auth = 0;
}

void IcqUser::load(DBInStream &in)
{
	IcqInfo::load(in);

	in >> passwd >> auth;
}

void IcqUser::save(DBOutStream &out)
{
	IcqInfo::save(out);
	
	out << passwd << auth;
}

void ProxyInfo::load(DBInStream &in)
{
	in >> host >> port >> username >> passwd >> resolve;
}

void ProxyInfo::save(DBOutStream &out)
{
	out << host << port << username << passwd << resolve;
}

IcqOption::IcqOption()
{
	hotKey = 0;
	port = 0;
}

IcqOption::~IcqOption()
{
}

void IcqOption::load(DBInStream &in)
{
	uint32 f;
	in >> f;
	flags = bitset<NR_USER_FLAGS>(f);
	in >> hotKey;

	for (int i = 0; i < NR_SOUNDS; ++i)
		in >> soundFiles[i];

	in >> host >> port;
	in >> autoReplyText >> quickReplyText;
	in >> autoStatus >> autoStatusTime;

	in >> skin;

	in >> proxyType;
	for (i = 0; i < NR_PROXY_TYPES; ++i)
		proxy[i].load(in);
}

void IcqOption::save(DBOutStream &out)
{
	out << flags.to_ulong();
	out << hotKey;

	for (int i = 0; i < NR_SOUNDS; i++)
		out << soundFiles[i];

	out << host << port;
	out << autoReplyText << quickReplyText;
	out << autoStatus << autoStatusTime;

	out << skin;

	out << proxyType;
	for (i = 0; i < NR_PROXY_TYPES; ++i)
		proxy[i].save(out);
}

void IcqOption::playSound(int sound, IcqContact *c)
{
	if (sound < 0 || sound >= NR_SOUNDS || !flags.test(UF_SOUND_ON))
		return;

	if (c && c->flags.test(CF_CUSTOMSOUND) &&
		sound < NR_CUSTOM_SOUNDS &&
		!c->soundFiles[sound].empty()) {
		playSound(c->soundFiles[sound].c_str());
	} else
		playSound(soundFiles[sound].c_str());
}


const char TEXT_STREAM_SEP = (const char) 0xfe;

TextOutStream &TextOutStream::operator <<(uint32 dw)
{
	char tmp[11];
	sprintf(tmp, "%lu", dw);
	return (*this << tmp);
}

TextOutStream &TextOutStream::operator <<(const char *str)
{
	if (cursor > text)
		*(cursor - 1) = TEXT_STREAM_SEP;

	int n = strlen(str) + 1;
	memcpy(cursor, str, n);
	cursor += n;
	return (*this);
}

TextInStream::TextInStream(const char *s)
{
	strncpy(text, s, sizeof(text));
	cursor = text;
}

TextInStream &TextInStream::operator >>(uint8 &b)

{
	uint32 dw;
	operator >>(dw);
	b = dw;
	return (*this);
}

TextInStream &TextInStream::operator >>(uint16 &w)
{
	uint32 dw;
	operator >>(dw);
	w = dw;
	return (*this);
}

TextInStream &TextInStream::operator >>(uint32 &dw)
{
	string s;
	*this >> s;
	dw = atol(s.c_str());
	return (*this);
}

TextInStream &TextInStream::operator >>(string &s)
{	
	char *p = cursor;

	if (*p == '\0')
		s = "";
	else {
		while (*p != TEXT_STREAM_SEP && *p)
			p++;

		s = string(cursor, p - cursor);

		cursor = p + 1;
	}

	return (*this);
}
