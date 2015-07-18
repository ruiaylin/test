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

#if !defined(AFX_MYICQCTRL_H__EB8DBF55_4F3E_41D2_8726_515944D25531__INCLUDED_)
#define AFX_MYICQCTRL_H__EB8DBF55_4F3E_41D2_8726_515944D25531__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyICQCtrl.h : header file
//

#include "OutBarCtrl.h"
#include "icqdb.h"

class IcqContact;
class DBInStream;
class DBOutStream;
class IcqSkin;

/////////////////////////////////////////////////////////////////////////////
// MyICQCtrl window

class MyICQCtrl : public OutBarCtrl, public DBSerialize {

// Construction
public:
	MyICQCtrl();

// Attributes
public:
	BOOL showOnlineOnly() {
		return onlineOnly;
	}
	void setShowOnlineOnly(int b) {
		onlineOnly = b;
		repaintInsideRect();
	}
	IcqContact *contact(int item) {
		return contact(selFolder, item);
	}
	IcqContact *contact(int folder, int item);

	void setSkin(IcqSkin *skin) {
		this->skin = skin;
	}

// Operations
public:
	void update(uint32 uin, BOOL move = FALSE);
	void addContact(int folder, IcqContact *c);
	void removeContact(uint32 uin);
	void blinkText(uint32 uin);
	void blinkImage(uint32 uin, BOOL blink = TRUE);

	void load(DBInStream &in);
	void save(DBOutStream &out);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyICQCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~MyICQCtrl();
		
private:
	class BlinkInfo {
	public:
		BlinkInfo(uint32 uin, int frame) {
			this->uin = uin;
			this->frame = frame;
		}
		uint32 uin;
		int frame;
	};
	typedef list<BlinkInfo> BlinkList;

	virtual int getMaxVisibleItem();
	virtual void drawBackground(CDC *pDC, CRect &rc);
	virtual void drawItemText(CDC *pDC, int i, COLORREF color);
	virtual void drawFolder(CDC *pDC, int i, BOOL pressed, BOOL hilight);
	virtual void drawScroll(CDC *pDC, int i, BOOL pressed);
	
	BOOL findItem(uint32 uin, int &folder, int &item);
	void doBlinkText();
	void doBlinkImage();
	
	BOOL onlineOnly;
	BlinkList blinkTextList;
	BlinkList blinkImageList;
	IcqSkin *skin;

	// Generated message map functions
protected:
	//{{AFX_MSG(MyICQCtrl)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYICQCTRL_H__EB8DBF55_4F3E_41D2_8726_515944D25531__INCLUDED_)
