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

#if !defined(AFX_OUTBARCTRL_H__72EE7AA0_C4F1_41A5_9D86_14B7023231C7__INCLUDED_)
#define AFX_OUTBARCTRL_H__72EE7AA0_C4F1_41A5_9D86_14B7023231C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutBarCtrl.h : header file
//

#include <vector>

using namespace std;

/*
 * OutBarCtrl listener that will be notified when certain message ocurrs
 */
class OutBarListener {
public:
	virtual void renamed(int obj, int index, const char *text) = 0;
	virtual void itemDragged(int item, int toFolder) = 0;
	virtual void selFolderChanged(int folder) = 0;
	virtual void itemClicked(int item) = 0;
	virtual void itemDoubleClicked(int item) = 0;
	virtual void rightButtonDown(int obj, int index) = 0;
};

class OutBarItem {
public:
	OutBarItem(const char *t, HICON ico)
		: text(t), icon(ico) {}
	virtual ~OutBarItem() {}

	CString text;
	HICON icon;
};

class OutBarFolder {
public:
	OutBarFolder(const char *t) : text(t) {
		scrollPos = 0;
	}
	~OutBarFolder();
	
	void addItem(const char *text, HICON icon);

	CString text;
	vector<OutBarItem *> items;
	int scrollPos;	
};

#define FOLDER_HEIGHT	22
#define LARGE_ICON_W	32
#define LARGE_ICON_H	32
#define SMALL_ICON_W	16
#define SMALL_ICON_H	16
#define ICON_OFFSET		5
#define LARGE_ITEM_H	48
#define SMALL_ITEM_H	22

// obj and index make up a HIT.
typedef int		HIT;
#define MAKEHIT(obj, index)		((obj << 16) + index)
#define HITOBJ(hit)				(hit >> 16)
#define HITINDEX(hit)			(hit & 0xffff)
#define HITNONE					MAKEHIT(HT_NONE, -1)

class OutBarEdit;

/////////////////////////////////////////////////////////////////////////////
// OutBarCtrl window

class OutBarCtrl : public CWnd {

// Construction
public:
	enum {
		HT_NONE,
		HT_FOLDER,
		HT_ITEM,
		HT_SCROLL
	};

	OutBarCtrl();

// Attributes
public:
	void setListener(OutBarListener *l) {
		listener = l;
	}
	int getFolderCount() {
		return folders.size();
	}
	int getSelFolder() {
		return selFolder;
	}
	void getFolderName(int i, CString &name);
	int getItemCount() {
		return getItemCount(selFolder);
	}
	int getItemCount(int folder);

	void setBgColor(COLORREF color) {
		bgColor = color;
		repaintInsideRect();
	}
	void setFontColor(COLORREF color) {
		fontColor = color;
		repaintInsideRect();
	}
	BOOL isLargeIconView() {
		return largeIconView;
	}
	void setIconView(BOOL large) {
		largeIconView = large;
		repaintInsideRect();
	}
	int hitObj() {
		return HITOBJ(lastHit);
	}
	int hitIndex() {
		return HITINDEX(lastHit);
	}

// Operations
public:
	void scrollToPos(int i) {
		scrollPos() = i;
		repaintInsideRect();
	}
	void removeItem(int i) {
		removeItem(selFolder, i);
	}
	void removeFolder(int i);
	void removeAllFolders();
	void removeItem(int folder, int i);
	void repaintInsideRect();
	void setSelFolder(int i);
	int addFolder(const char *text);
	void addItem(int folder, const char *text, HICON icon);
	void insertItem(int folder, int pos, OutBarItem *item);

	void addFolderUI();
	void renameUI();
	void resizeEditCtrl();
	void onEditFinished();
	void onEditCanceled();

protected:
	enum {
		SCROLL_DIR_UP,
		SCROLL_DIR_DOWN
	};

	virtual int getMaxItem();
	virtual void drawBackground(CDC *pDC, CRect &rc);
	virtual void drawItemText(CDC *pDC, int i, COLORREF color);
	virtual void drawFolder(CDC *pDC, int i, BOOL pressed = FALSE, BOOL hilight = FALSE);
	virtual void drawScroll(CDC *pDC, int i, BOOL pressed = FALSE);

	int getCountPerPage();
	int getMaxVisibleItem();
	void getFolderRect(CRect &rc, int i);
	void getInsideRect(CRect &rc);
	void getItemRect(CRect &rc, int i);
	void getIconRect(CRect &rc, int i);
	void getLabelRect(CRect &rc, int i);
	void getScrollRect(CRect &rc, int i);
	void drawAll(CDC *pDC);
	void drawItem(CDC *pDC, int i, BOOL redraw = FALSE);
	void drawItemImage(CDC *pDC, int i, CRect &rc);
	void drawFolderText(CDC *pDC, int i, CRect &rc);
	void animateFolder(int from, int to);
	void hilightItem(CDC *pDC, int i, BOOL pressed = FALSE);
	HIT hitTest(const CPoint &pt);
	BOOL canScroll(int dir);
	void scroll(int dir, int delta = 1);
	int &scrollPos();

	OutBarListener *listener;
	vector<OutBarFolder *> folders;
	int selFolder;
	COLORREF bgColor;
	COLORREF fontColor;
	HIT lastHit;
	HIT pressedHit;
	int clickedItem;
	int dragItem;
	BOOL largeIconView;
	OutBarEdit *editCtrl;

	HCURSOR currentCursor;
	HCURSOR handCursor;
	HCURSOR forbiddenCursor;
	HCURSOR dragCursor;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OutBarCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~OutBarCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(OutBarCtrl)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTBARCTRL_H__72EE7AA0_C4F1_41A5_9D86_14B7023231C7__INCLUDED_)
