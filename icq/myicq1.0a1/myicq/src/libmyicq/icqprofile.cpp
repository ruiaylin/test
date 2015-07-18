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

#include "icqtypes.h"
#include "icqprofile.h"
#include "ndes.h"
#include <stdio.h>
#include <ctype.h>
#include <list>

static char *trim(char *str, int n)
{
	while (isspace(*str))
		++str;
	
	char *p = str + n;
	while (p > str && isspace(*--p));
	*++p = '\0';
		
	return str;
}

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


void IcqProfile::setSectionName(const char *section)
{
	sectionName = section;
	pairs.clear();
	
	FILE *file = fopen(fileName.c_str(), "r");
	if (!file)
		return;

	char line[1024];
	char name[1024];
	char val[1024];
	char *src, *dst;
	
	while (fgets(line, sizeof(line), file)) {
		if (*line != '[')
			continue;
			
		dst = name;
		src = line;
		while (*++src != ']' && *src)
			*dst++ = *src;
		if (*src) {
			*dst = '\0';
			dst = trim(name, dst - name);
			if (strcasecmp(dst, section) == 0) {
				sectionName = dst;
				break;
			}
		}
	}
	
	while (fgets(line, sizeof(line), file)) {
		if (*line == '[')
			break;
			
		dst = name;
		src = line;
		while (*src != '=' && *src)
			*dst++ = *src++;
		if (!*src)
			continue;
			
		*dst = '\0';
		int n = dst - name;
		dst = val;
		while (*++src && *src != '\n')
			*dst++ = *src;
		*dst = '\0';
			
		pairs[trim(name, n)] = trim(val, dst - val);
	}
				
	fclose(file);
}

void IcqProfile::flush()
{
	list<string> front, back;
	
	FILE *file = fopen(fileName.c_str(), "r");
	if (file) {
		char line[1024];
		char name[1024];
	
		while (fgets(line, sizeof(line), file)) {
			if (*line == '[') {
				char *dst = name;
				char *src = line;
				while (*++src != ']' && *src)
					*dst++ = *src;
				if (*src) {
					*dst = '\0';
					if (strcasecmp(trim(name, dst - name), sectionName.c_str()) == 0)
						break;
				}
			}
			front.push_back(line);
		}
		while (fgets(line, sizeof(line), file)) {
			if (*line == '[') {
				back.push_back(line);
				break;
			}
		}
		while (fgets(line, sizeof(line), file))
			back.push_back(line);
		fclose(file);
	}
		
	file = fopen(fileName.c_str(), "w");
	if (!file)
		return;

	list<string>::iterator it;
	for (it = front.begin(); it != front.end(); ++it)
		fputs((*it).c_str(), file);

	// write section name
	fprintf(file, "[%s]\n", sectionName.c_str());
	// write key/value pairs
	map<string, string>::iterator iter;
	for (iter = pairs.begin(); iter != pairs.end(); ++iter)
		fprintf(file, "%s = %s\n", (*iter).first.c_str(), (*iter).second.c_str());
	fprintf(file, "\n");
	
	for (it = back.begin(); it != back.end(); ++it)
		fputs((*it).c_str(), file);
		
	fclose(file);
	pairs.clear();
}

const char *IcqProfile::readString(const char *key, const char *def)
{
	map<string, string>::iterator it = pairs.find(key);
	if (it == pairs.end())
		return def;
	return (*it).second.c_str();
}

int IcqProfile::readInt(const char *key, int def)
{
	const char *val = readString(key, NULL);
	return (val ? atoi(val) : def);
}

void IcqProfile::readPassword(const char *key, string &passwd)
{
	passwd = "";
	const char *val = readString(key, NULL);
	if (!val)
		return;

	char buf[256];
	char *dst = buf;
	const char *src = val;
	while (*src) {
		char c = (char2hex(*src++) << 4);
		if (*src)
			c |= char2hex(*src++);
		*dst++ = c;
	}
	int n = (dst - buf) / 8;
	dst = buf;
	for (int i = 0; i < n; ++i) {
		dedes(dst);
		dst += 8;
	}
	*dst = '\0';

	passwd = buf;
}

void IcqProfile::writeInt(const char *key, int val)
{
	char buf[128];
	sprintf(buf, "%d", val);
	writeString(key, buf);
}

void IcqProfile::writePassword(const char *key, const char *passwd)
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
