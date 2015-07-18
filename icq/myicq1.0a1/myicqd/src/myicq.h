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

#ifndef _MYICQ_H
#define _MYICQ_H

#include "icqtypes.h"
#include <time.h>
#include <string>

using namespace std;

#define MYICQ_PORT_DEFAULT	8000
#define NR_DB_QUERY			4

struct DB_INFO {
	string host;
	uint16 port;
	string db;
	string user;
	string passwd;
};

struct OPTIONS {
	// network
	uint32 myicqIP;
	uint16 myicqPort;

	// log
	int logLevel;
	string logFile;

	// mysql
	DB_INFO dbInfo;

	int enableRegister;
	int enableS2S;

	// server
	string domain;
	string desc;

	// plugins
	string pluginDir;
};


// Function prototypes...
bool myicqInit();
void myicqDestroy();
void initArgs(int argc, char *argv[]);

void pulse();
void handlePacket();
void handleDNS();
void handleDBQuery(int i);
void handleDBUpdate();

// External variables...
extern OPTIONS _ops;
extern time_t curTime;


#endif
