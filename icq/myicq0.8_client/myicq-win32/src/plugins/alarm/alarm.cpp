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

// alarm.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "alarm.h"
#include "plugin.h"
#include "icqlinkbase.h"
#include "icqprofile.h"
#include "ConfigDlg.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

#define ICQ_EXPORT	extern "C" __declspec(dllexport)

static BOOL alarmEnabled = FALSE;
static UINT timerID = 0;
static string alarmText;
static CTime alarmTime;
static HICON alarmIcon = NULL;
static IcqLinkBase *icqLink = NULL;


static VOID CALLBACK onTimer(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (idEvent != timerID)
		return;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	KillTimer(hwnd, idEvent);
	if (alarmEnabled) {
		icqLink->popupMsg(alarmIcon, alarmText.c_str(), 0);
		PlaySound(MAKEINTRESOURCE(IDR_ALARM), AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
	}
}

static void setTimer(CTime &t)
{
	CTime cur = CTime::GetCurrentTime();
	if (t >= cur) {
		KillTimer(NULL, timerID);
		CTimeSpan span = t - cur;
		timerID = SetTimer(NULL, 0, span.GetTotalSeconds() * 1000, onTimer);
	}
}

ICQ_EXPORT int getType()
{
	return ICQ_PLUGIN_TOY;
}

ICQ_EXPORT void *getNameIcon(string &name)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString str;
	str.LoadString(IDS_ALARM);
	name = str;

	if (!alarmIcon)
		alarmIcon = AfxGetApp()->LoadIcon(IDI_ALARM);
	return (void *) alarmIcon;
}

ICQ_EXPORT bool init(IcqLinkBase *link, IcqProfile *profile)
{
	icqLink = link;

	alarmEnabled = profile->readBool("enable");
	int year = profile->readInt("year");
	int month = profile->readInt("month");
	int day = profile->readInt("day");
	int hour = profile->readInt("hour");
	int min = profile->readInt("minute");
	int sec = profile->readInt("second");
	profile->readString("text", alarmText);

	if (year <= 0)
		alarmTime = CTime::GetCurrentTime();
	else
		alarmTime = CTime(year, month, day, hour, min, sec);
	if (alarmEnabled)
		setTimer(alarmTime);

	return true;
}

ICQ_EXPORT void configure(IcqProfile *profile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CConfigDlg dlg;
	dlg.m_enable = alarmEnabled;
	dlg.m_date = alarmTime;
	dlg.m_time = alarmTime;
	dlg.m_text = alarmText.c_str();

	if (dlg.DoModal() != IDOK)
		return;

	alarmEnabled = dlg.m_enable;
	alarmTime = CTime(dlg.m_date.GetYear(), dlg.m_date.GetMonth(), dlg.m_date.GetDay(),
		dlg.m_time.GetHour(), dlg.m_time.GetMinute(), dlg.m_time.GetSecond());
	alarmText = dlg.m_text;

	profile->writeBool("enable", alarmEnabled ? true : false);
	profile->writeInt("year", alarmTime.GetYear());
	profile->writeInt("month", alarmTime.GetMonth());
	profile->writeInt("day", alarmTime.GetDay());
	profile->writeInt("hour", alarmTime.GetHour());
	profile->writeInt("minute", alarmTime.GetMinute());
	profile->writeInt("second", alarmTime.GetSecond());
	profile->writeString("text", alarmText.c_str());

	if (alarmEnabled)
		setTimer(alarmTime);
}

/////////////////////////////////////////////////////////////////////////////
// CAlarmApp

BEGIN_MESSAGE_MAP(CAlarmApp, CWinApp)
	//{{AFX_MSG_MAP(CAlarmApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmApp construction

CAlarmApp::CAlarmApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAlarmApp object

CAlarmApp theApp;
