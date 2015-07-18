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

/*
 * These are utility classes used to read or write a configeration file(sometimes
 * called ini file). The configeration file has the following format:
 *
 * [section]
 * key = value
 */

#ifndef _PROFILE_H
#define _PROFILE_H

#include <string>

using namespace std;


/*
 * Read a configeration file
 */
class ProfileReader {
public:
	// Read an integer value from a key.
	virtual int readInt(const char *key, int def) = 0;
	// Read a string value from a key.
	virtual const char *readString(const char *key, const char *def) = 0;
	// Read a password string from a key. The password is decrypted
	// when reading from the file.
	virtual void readPassword(const char *key, string &passwd) = 0;
};

/*
 * Write a configeration file
 */
class ProfileWriter {
public:
	// Write an integer value to a key.
	virtual void writeInt(const char *key, int val) = 0;
	// Write a string value to a key.
	virtual void writeString(const char *key, const char *val) = 0;
	// Write a password value to a key. The password is encrypted when
	// writing to the file.
	virtual void writePassword(const char *key, const char *passwd) = 0;
};

#endif
