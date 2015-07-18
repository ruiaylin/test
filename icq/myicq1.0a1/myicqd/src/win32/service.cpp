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

#include <windows.h>
#include <iostream.h>
#include "myicq.h"
#include "log.h"


#define SERVICE_NAME			"myicqd"

static SERVICE_STATUS			serviceStatus;
static SERVICE_STATUS_HANDLE	hServiceStatus;

bool myicqStart();

static void setStatus(DWORD status)
{
	serviceStatus.dwCurrentState = status;
	SetServiceStatus(hServiceStatus, &serviceStatus);
}

static VOID WINAPI serviceCtrlHandler(DWORD opcode)
{
	switch (opcode) {
	case SERVICE_CONTROL_STOP:
		setStatus(SERVICE_STOPPED);
		break;
	}
}

static VOID WINAPI serviceMain(DWORD argc, LPTSTR *argv)
{
	ZeroMemory(&serviceStatus, sizeof(serviceStatus));
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	hServiceStatus = RegisterServiceCtrlHandler(SERVICE_NAME, serviceCtrlHandler);
	if (!hServiceStatus) {
		LOG(1) ("RegisterServiceCtrlHandler() failed\n");
		return;
	}

	// Starting myicqd...
	setStatus(SERVICE_START_PENDING);

	if (!myicqStart()) {
		setStatus(SERVICE_STOPPED);
		return;
	}

	// myicqd is now started
	setStatus(SERVICE_RUNNING);

	handlePacket();

	myicqDestroy();
}


bool installService()
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCM) {
		cerr << "Can not open SCM" << endl;
		return false;
	}

	char pathName[_MAX_PATH];
	GetModuleFileName(NULL, pathName, _MAX_PATH);
	
	SC_HANDLE hService = CreateService(
		hSCM,
		SERVICE_NAME,
		"MyICQ",
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		pathName,
		NULL,
		NULL,
		NULL,		// No dependence, since mysqld may run on another server
		NULL,
		NULL
	);

	bool ret = false;
	
	if (hService) {
		ret = true;
		CloseServiceHandle(hService);
	} else
		cerr << "CreateService() failed" << endl;

	CloseServiceHandle(hSCM);

	return ret;
}

bool removeService()
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, 0);
	if (!hSCM) {
		cerr << "Can not open SCM" << endl;
		return false;
	}

	bool ret = false;

	SC_HANDLE hService = OpenService(hSCM, SERVICE_NAME, DELETE);
	if (hService) {
		if (DeleteService(hService))
			ret = true;
		else
			cerr << "DeleteService() failed" << endl;
		CloseServiceHandle(hService);
	} else
		cerr << "OpenService() failed" << endl;

	CloseServiceHandle(hSCM);
	return ret;
}

bool startService()
{
	static SERVICE_TABLE_ENTRY dispatchTable[] = {
		{ SERVICE_NAME, serviceMain },
		{ NULL, NULL }
	};

	if (!StartServiceCtrlDispatcher(dispatchTable))
		return false;

	return true;
}
