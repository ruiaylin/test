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

#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>

#define LOG(level)	if (level <= Log::logLevel) Log::print


class Log {
public:
	static void open(const char *file, int level);
	static void closeLog();
	static void print(const char *fmt, ...);

	static int logLevel;

private:
	static FILE *logFile;
};

#endif
