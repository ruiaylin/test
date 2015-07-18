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

// myicq.h : main header file for the MYICQ application
//

#if !defined(AFX_MYICQ_H__B7BB3A1D_B036_4D10_B30A_98BCCA7836E6__INCLUDED_)
#define AFX_MYICQ_H__B7BB3A1D_B036_4D10_B30A_98BCCA7836E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "icqtypes.h"

#define	NR_PICS_PER_FACE	3

#ifdef _DEBUG
#define NR_FACES	5
#else
#define NR_FACES	85
#endif

#define TOTAL_FACES		(NR_PICS_PER_FACE * NR_FACES)
#define NR_EMOTIONS		50

// Text data files
#define DATA_BLOOD		"blood.txt"
#define DATA_GENDER		"gender.txt"
#define DATA_COUNTRY	"country.txt"
#define DATA_PROVINCE	"province.txt"
#define DATA_OCCUPATION	"occupation.txt"
#define DATA_HOST		"host.txt"

enum {
	ICON_SYSMSG,
	ICON_FOLDER,
};

class IconManager {
public:
	IconManager() {
		imageList = NULL;
		icons = NULL;
	}
	~IconManager();

	void setImageList(CImageList *il);
	HICON getIcon(int i);

private:
	CImageList *imageList;
	HICON *icons;
};


/////////////////////////////////////////////////////////////////////////////
// CIcqApp:
// See myicq.cpp for the implementation of this class
//

class IcqMsg;
class IcqGroup;

class CIcqApp : public CWinApp
{
public:
	CIcqApp();

	int iconIndex(int icon) {
		return (NR_FACES * NR_PICS_PER_FACE + icon);
	}
	int getFaceIndex(int pic, int status = 0) {
		return (pic + status * NR_FACES);
	}
	HICON getLargeFace(int pic, int status = 0) {
		return largeFaces.getIcon(getFaceIndex(pic, status));
	}
	HICON getSmallFace(int pic, int status = 0) {
		return smallFaces.getIcon(getFaceIndex(pic, status));
	}
	HICON getGroupIcon(IcqGroup *g);

	void getUserDir(CString &dir, QID &qid);
	BOOL loadText(CComboBox &cb, LPCTSTR fileName);

	HICON m_hIcon;
	HICON groupIcon;
	CImageList largeImageList;
	CImageList smallImageList;
	CImageList emotionImageList;
	CString rootDir;

	BOOL m_bWin4;
	int m_nDefFont;
	CDC m_dcScreen;

private:
	BOOL initGraphics();

	IconManager largeFaces;
	IconManager smallFaces;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcqApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIcqApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CIcqApp *myicq;

inline CIcqApp *getApp() {
	return myicq;
}

void getMsgText(IcqMsg *msg, CString &str);
int myMessageBox(UINT text, UINT title, CWnd *parent, UINT type = MB_OK);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYICQ_H__B7BB3A1D_B036_4D10_B30A_98BCCA7836E6__INCLUDED_)
