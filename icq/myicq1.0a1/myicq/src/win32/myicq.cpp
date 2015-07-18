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
#include "RichEditCtrlEx.h"
#include "icqconfig.h"
#include "icqgroup.h"

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
		{
			str = msg->text.c_str();
			uint8 mask = MF_RELAY | MF_RECEIVED;
			if ((msg->flags & mask) == mask && !msg->qid.isAdmin()) {
				CString tmp;
				tmp.LoadString(IDS_SERVER_RELAY);
				str += "\r\n";
				str += tmp;
			}
		}
	}
}

IconManager::~IconManager()
{
	if (icons && imageList) {
		int n = imageList->GetImageCount();
		for (int i = 0; i < n; ++i) {
			if (icons[i])
				DestroyIcon(icons[i]);
		}
	}
	if (icons)
		delete []icons;
}

void IconManager::setImageList(CImageList *il)
{
	imageList = il;
	int n = il->GetImageCount();
	icons = new HICON[n];
	ZeroMemory(icons, sizeof(HICON) * n);
}

HICON IconManager::getIcon(int i)
{
	if (!imageList || !icons || i < 0 || i >= imageList->GetImageCount())
		return NULL;

	HICON icon = icons[i];
	if (!icon) {
		icon = imageList->ExtractIcon(i);
		icons[i] = icon;
	}
	return icon;
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

	DWORD dwVersion = ::GetVersion();
	m_bWin4 = (BYTE)dwVersion >= 4;
	m_nDefFont = (m_bWin4) ? DEFAULT_GUI_FONT : ANSI_VAR_FONT;
	m_dcScreen.Attach(::GetDC(NULL));

	myicq = this;
}

HICON CIcqApp::getGroupIcon(IcqGroup *g)
{
	HICON icon = (HICON) g->getIcon();
	if (!icon)
		icon = groupIcon;
	return icon;
}

void CIcqApp::getUserDir(CString &dir, QID &qid)
{
	dir = rootDir + qid.toString() + '\\';
	CreateDirectory(dir, NULL);
}

BOOL CIcqApp::loadText(CComboBox &cb, LPCTSTR fileName)
{
	TRY
	{
		CStdioFile file(rootDir + "data\\" + fileName, CFile::modeRead);

		CString str;
		while (file.ReadString(str))
			cb.AddString(str);
	}
	CATCH(CFileException, e)
	{
		TRACE1("Error read file %s\n", fileName);
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

BOOL CIcqApp::initGraphics()
{
	m_hIcon = LoadIcon(IDR_MAINFRAME);

	largeImageList.Create(32, 32, ILC_COLORDDB | ILC_MASK, TOTAL_FACES, 0);
	smallImageList.Create(16, 16, ILC_COLORDDB | ILC_MASK, TOTAL_FACES, 0);
	COLORREF crMask = RGB(0, 128, 128);

	for (int i = 1; i <= NR_PICS_PER_FACE; i++) {
		for (int j = 1; j <= NR_FACES; j++) {
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

	groupIcon = LoadIcon(IDI_GROUP);

	smallImageList.Add(LoadIcon(IDI_SYSMSG));
	smallImageList.Add(LoadIcon(IDI_FOLDER));

	largeFaces.setImageList(&largeImageList);
	smallFaces.setImageList(&smallImageList);

	CBitmap bm;
	bm.LoadBitmap(IDB_EMOTIONS);
	emotionImageList.Create(20, 20, ILC_COLOR8 | ILC_MASK, NR_EMOTIONS, 0);
	emotionImageList.Add(&bm, crMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIcqApp object

CIcqApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIcqApp initialization

BOOL CIcqApp::InitInstance()
{
	if (!AfxInitRichEditEx())
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

	if (!initGraphics())
		return FALSE;

	CIcqDlg *dlg = new CIcqDlg;
	m_pMainWnd = dlg;
	dlg->Create(IDD_MYICQ_DIALOG);

	if (!dlg->doLogin())
		return FALSE;

	return TRUE;
}
