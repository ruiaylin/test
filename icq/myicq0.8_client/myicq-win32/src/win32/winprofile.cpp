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

#include "stdafx.h"
#include "winprofile.h"
#include "ndes.h"

static char hex2char(char c)
{
	if (c < 10)
		return (c + '0');
	return (c - 10) + 'a';
}

static char char2hex(char c)
{
	if (isdigit(c))
		return (c - '0');
	return (c - 'a' + 10);
}

bool WinProfile::readBool(const char *key)
{
	string val;
	readString(key, val);
	return (val.compare("y") == 0);
}

int WinProfile::readInt(const char *key)
{
	return GetPrivateProfileInt(sectionName, key, 0, fileName);
}

void WinProfile::readString(const char *key, string &val)
{
	char str[256];
	GetPrivateProfileString(sectionName, key, "", str, sizeof(str), fileName);
	val = str;
}

void WinProfile::readPassword(const char *key, string &passwd)
{
	string val;
	readString(key, val);

	char buf[256];
	char *p = buf;
	int n = val.length();
	for (int i = 0; i < n; ++i) {
		char c = (char2hex(val[i++]) << 4);
		c |= char2hex(val[i]);
		*p++ = c;
	}
	n /= 16;
	p = buf;
	for (i = 0; i < n; ++i) {
		dedes(p);
		p += 8;
	}
	*p = '\0';

	passwd = buf;
}

void WinProfile::writeBool(const char *key, bool b)
{
	const char *val = (b ? "y" : "n");
	writeString(key, val);
}

void WinProfile::writeInt(const char *key, int val)
{
	CString str;
	str.Format("%d", val);
	writeString(key, str);
}

void WinProfile::writeString(const char *key, const char *val)
{
	WritePrivateProfileString(sectionName, key, val, fileName);
}

void WinProfile::writePassword(const char *key, const char *passwd)
{
	char buf[256];
	int n = strlen(passwd);
	memcpy(buf, passwd, n);
	int i = n % 8;
	if (i) {
		i = 8 - i;
		while (i-- > 0)
			buf[n++] = 0;
	}

	char *p = buf;
	for (i = 0; i < n / 8; ++i) {
		endes(p);
		p += 8;
	}

	char val[256];
	p = val;
	for (i = 0; i < n; ++i) {
		*p++ = hex2char((buf[i] & 0xf0) >> 4);
		*p++ = hex2char(buf[i] & 0x0f);
	}
	*p = '\0';

	writeString(key, val);
}
