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
class IcqGroup;
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
	void setShowOnlineOnly(BOOL b);
	IcqContact *contact(int item) {
		return contact(selFolder, item);
	}
	IcqContact *contact(int folder, int item);
	IcqGroup *group(int item);

	void setSkin(IcqSkin *skin) {
		this->skin = skin;
	}

// Operations
public:
	void update(QID &qid, BOOL move = FALSE);
	void addContact(int folder, IcqContact *c);
	void removeContact(QID &qid);
	void addGroup(IcqGroup *g);
	void removeGroup(uint32 id);

	void blinkText(QID &qid);
	void blinkImage(QID &qid, BOOL blink = TRUE);
	void blinkImage(uint32 id, BOOL blink = TRUE);

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
		BlinkInfo(QID &qid, int frame) {
			this->qid = qid;
			this->frame = frame;
			id = 0;
		}
		BlinkInfo(uint32 id) {
			this->id = id;
			frame = 0;
		}

		QID qid;
		uint32 id;
		int frame;
	};
	typedef list<BlinkInfo> BlinkList;

	virtual int getMaxItem();
	virtual void drawBackground(CDC *pDC, CRect &rc);
	virtual void drawItemText(CDC *pDC, int i, COLORREF color);
	virtual void drawFolder(CDC *pDC, int i, BOOL pressed, BOOL hilight);
	virtual void drawScroll(CDC *pDC, int i, BOOL pressed);

	BOOL findItem(QID &qid, int &folder, int &item);
	BOOL findGroup(uint32 id, int &folder, int &item);
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
