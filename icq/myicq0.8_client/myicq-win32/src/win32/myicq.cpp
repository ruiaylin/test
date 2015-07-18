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

// myicq.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "myicq.h"
#include "myicqDlg.h"
#include "icqconfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int myMessageBox(UINT text, UINT title, CWnd *parent, UINT type)
{
	CString strText, strTitle;
	strText.LoadString(text);
	strTitle.LoadString(title);
	return parent->MessageBox(strText, strTitle, type);
}

void getMsgText(IcqMsg *msg, CString &str)
{
	switch (msg->type) {
	case MSG_AUTH_REQUEST:
		{
			TextInStream in(msg->text.c_str());
			uint8 face;
			string nick, text;
			in >> face >> nick >> text;
			str.Format(IDS_AUTH_REQUEST, text.c_str());
		}
		break;

	case MSG_AUTH_ACCEPTED:
		str.LoadString(IDS_AUTH_ACCEPTED);
		break;

	case MSG_AUTH_REJECTED:
		str.Format(IDS_AUTH_REJECTED, msg->text.c_str());
		break;

	case MSG_ADDED:
		str.LoadString(IDS_MSG_ADDED);
		break;

	default:
		str = msg->text.c_str();
		if (msg->flags & MF_RELAY) {
			CString tmp;
			tmp.LoadString(IDS_SERVER_RELAY);
			str += "\r\n";
			str += tmp;
		}
	}
}

CIcqApp *myicq;

/////////////////////////////////////////////////////////////////////////////
// CIcqApp

BEGIN_MESSAGE_MAP(CIcqApp, CWinApp)
	//{{AFX_MSG_MAP(CIcqApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcqApp construction

CIcqApp::CIcqApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_hIcon = NULL;

	myicq = this;
}

void CIcqApp::setDBDir(uint32 uin)
{
	CString dir;
	dir.Format("%s%lu\\", rootDir, uin);
	CreateDirectory(dir, NULL);
	IcqDB::setDir(dir);

	profile.fileName = dir + "plugins.cfg";
}

IcqProfile *CIcqApp::getProfile(LPCTSTR name)
{
	profile.sectionName = name;
	return &profile;
}

void CIcqApp::initData()
{
	CString str;

	genderNames.Add("-");
	str.LoadString(IDS_MALE);
	genderNames.Add(str);
	str.LoadString(IDS_FEMALE);
	genderNames.Add(str);

	bloodNames.Add("-");
	bloodNames.Add("A");
	bloodNames.Add("B");
	bloodNames.Add("O");
	bloodNames.Add("AB");
	str.LoadString(IDS_OTHERS);
	bloodNames.Add(str);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIcqApp object

CIcqApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIcqApp initialization

BOOL CIcqApp::InitInstance()
{
	if (!AfxInitRichEdit())
		return FALSE;

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	initData();

	char fileName[MAX_PATH];
	if (!GetModuleFileName(NULL, fileName, sizeof(fileName)))
		return FALSE;
	char *p = strrchr(fileName, '\\');
	if (p)
		*(p + 1) = '\0';
	rootDir = fileName;

	CString dir = rootDir + "config\\";
	CreateDirectory(dir, NULL);
	IcqConfig::setDir(dir);

	m_hIcon = LoadIcon(IDR_MAINFRAME);

#ifdef _DEBUG
	nrFaces = 5;
#else
	nrFaces = 85;
#endif
	int n = nrFaces * NUM_PICS_PER_FACE;
	largeImageList.Create(32, 32, ILC_COLOR16 | ILC_MASK, n, 0);
	smallImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, n, 0);
	COLORREF crMask = RGB(0, 128, 128);

	for (int i = 1; i <= NUM_PICS_PER_FACE; i++) {
		for (int j = 1; j <= nrFaces; j++) {
			CString strFile;
			strFile.Format(rootDir + "face\\%d-%d.bmp", j, i);
			HBITMAP hbmLarge = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), strFile,
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			HBITMAP hbmSmall = (HBITMAP) ::CopyImage(hbmLarge, IMAGE_BITMAP, 16, 16, 0);

			largeImageList.Add(CBitmap::FromHandle(hbmLarge), crMask);
			smallImageList.Add(CBitmap::FromHandle(hbmSmall), crMask);
			DeleteObject(hbmLarge);
			DeleteObject(hbmSmall);
		}
	}

	smallImageList.Add(LoadIcon(IDI_SYSMSG));
	smallImageList.Add(LoadIcon(IDI_FOLDER));

	CIcqDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
