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

#include "myicq.h"
#include "service.h"
#include "log.h"
#include "groupplugin.h"
#include <windows.h>


static DWORD WINAPI dnsThread(LPVOID)
{
	handleDNS();
	return 0;
}

static DWORD WINAPI dbUpdateThread(LPVOID)
{
	handleDBUpdate();
	return 0;
}

static DWORD WINAPI dbQueryThread(LPVOID i)
{
	handleDBQuery((int) i);
	return 0;
}

static DWORD WINAPI pulseThread(LPVOID)
{
	pulse();
	return 0;
}

bool myicqStart()
{
	if (!myicqInit())
		return false;

	DWORD id;
	int i;

	// Creating threads...
	CreateThread(NULL, 0, pulseThread, NULL, 0, &id);

	// DNS
	CreateThread(NULL, 0, dnsThread, NULL, 0, &id);

	CreateThread(NULL, 0, dbUpdateThread, NULL, 0, &id);
	for (i = 0; i < NR_DB_QUERY; ++i)
		CreateThread(NULL, 0, dbQueryThread, (LPVOID) i, 0, &id);

	return true;
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsaData) != 0)
		return 1;

	initArgs(argc, argv);

#ifndef _DEBUG
	if (!startService())
#endif
	{
		// We are not in service mode
		if (myicqStart()) {
			handlePacket();
			myicqDestroy();
		}
	}

	WSACleanup();
	return 0;
}
