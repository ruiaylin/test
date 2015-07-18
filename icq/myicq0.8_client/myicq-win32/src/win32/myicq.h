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
#include "winprofile.h"

#define	NUM_PICS_PER_FACE	3

enum {
	ICON_SYSMSG,
	ICON_FOLDER,
};

/////////////////////////////////////////////////////////////////////////////
// CIcqApp:
// See myicq.cpp for the implementation of this class
//

class IcqMsg;
class IcqLink;

class CIcqApp : public CWinApp
{
public:
	CIcqApp();

	int getImageIndex(int pic, int state = 0) {
		return (pic + state * nrFaces);
	}
	int iconIndex(int icon) {
		return (nrFaces * NUM_PICS_PER_FACE + icon);
	}
	void setDBDir(DWORD uin);
	IcqProfile *getProfile(LPCTSTR name);

	HICON m_hIcon;
	CImageList largeImageList;
	CImageList smallImageList;
	int nrFaces;
	CString rootDir;

	CStringArray genderNames;
	CStringArray bloodNames;

private:
	void initData();

	WinProfile profile;

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
