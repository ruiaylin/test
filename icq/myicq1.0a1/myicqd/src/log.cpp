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

#include <stdarg.h>
#include <time.h>
#include "log.h"
#include "myicq.h"

#define BUFSIZE		4096

#ifdef _WIN32
#define snprintf	_snprintf
#endif

FILE *Log::logFile = NULL;
int Log::logLevel = 0;


static const char *logTime(time_t t)
{
    static char buf[128];
    static time_t last_t = 0;

    if (t != last_t) {
		struct tm *tm = localtime(&t);
		strftime(buf, 127, "%Y/%m/%d %H:%M:%S", tm);
		last_t = t;
    }
    return buf;
}

void Log::open(const char *file, int level)
{
	logLevel = level;

	if (logFile)
		closeLog();

	bool b = (file && *file);
	if (b)
		logFile = fopen(file, "a+");
	if (!logFile) {
		logFile = stderr;
		if (b)
			LOG(1) ("Error open log file: %s\n", file);
	}
}

void Log::closeLog()
{
	if (logFile && logFile != stderr) {
		fclose(logFile);
		logFile = NULL;
	}
}

void Log::print(const char *fmt, ...)
{
	char buf[BUFSIZE];
	
	va_list args;
	va_start(args, fmt);

	snprintf(buf, sizeof(buf), "%s| %s", logTime(curTime), fmt);
	vfprintf(logFile, buf, args);

	fflush(logFile);

	va_end(args);
}
