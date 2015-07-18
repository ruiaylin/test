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

// MyICQCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "MyICQCtrl.h"
#include "icqlink.h"
#include "icqclient.h"
#include "icqskin.h"
#include "icqgroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class ContactItem : public OutBarItem {
public:
	ContactItem(IcqContact *c)
		: OutBarItem(c->nick.c_str(), getApp()->getLargeFace(c->face, c->status)),
		contact(c) {}
	int compare(ContactItem *item);

	IcqContact *contact;
};

class GroupItem : public OutBarItem {
public:
	GroupItem(IcqGroup *g)
		: OutBarItem(g->name.c_str(), getApp()->getGroupIcon(g)),
		group(g) {}

	IcqGroup *group;
};

/*
 * Compare two items based on the contact's nick name and current status
 */
int ContactItem::compare(ContactItem *item)
{
	int s1 = contact->status;
	int s2 = item->contact->status;
	if (s1 != s2) {
		switch (s1) {
		case STATUS_ONLINE:
			return -1;
		case STATUS_OFFLINE:
			return 1;
		case STATUS_AWAY:
			return (s2 == STATUS_ONLINE ? 1 : -1);
		}
	}
	return text.CollateNoCase(item->text);
}


// The colors that will be shown circly when a contact is online
static COLORREF blinkColors[] = {
	RGB(255, 0, 0),
	RGB(255, 255, 0),
	RGB(0, 255, 255),
	RGB(0, 255, 0),
	RGB(0, 0, 255),
};
#define NR_BLINK_COLORS	(sizeof(blinkColors) / sizeof(*blinkColors))

// The offsets that the icon will swing circly when a message comes.
#define BLINK_OFFSET	3
static CPoint blinkOffsets[] = {
	CPoint(0, 0),
	CPoint(BLINK_OFFSET, BLINK_OFFSET),
	CPoint(0, 0),
	CPoint(-BLINK_OFFSET, BLINK_OFFSET),
};
#define NR_BLINK_OFFSETS	(sizeof(blinkOffsets) / sizeof(*blinkOffsets))

#define IDT_BLINK_TEXT		2001
#define IDT_BLINK_IMAGE		2002

/////////////////////////////////////////////////////////////////////////////
// MyICQCtrl

MyICQCtrl::MyICQCtrl()
{
	onlineOnly = FALSE;
	skin = NULL;
}

MyICQCtrl::~MyICQCtrl()
{
}

/*
 * Return Contact at folder and item
 */
IcqContact *MyICQCtrl::contact(int folder, int item)
{
	int n = folders.size();
	if (folder >= 0 && folder < n && folder == n - 3)
		return NULL;
	return ((ContactItem *) folders[folder]->items[item])->contact;
}

IcqGroup *MyICQCtrl::group(int item)
{
	if (selFolder != folders.size() - 3)
		return NULL;
	return ((GroupItem *) folders[selFolder]->items[item])->group;
}

void MyICQCtrl::setShowOnlineOnly(BOOL b)
{
	onlineOnly = b;
	if (b) {
		for (int i = getFolderCount(); --i >= 0;)
			folders[i]->scrollPos = 0;
	}
	repaintInsideRect();
}

/*
 * Reorder and redraw the corresponding item
 */
void MyICQCtrl::update(QID &qid, BOOL move)
{
	int folder, item;
	if (findItem(qid, folder, item)) {
		vector<OutBarItem *> &items = folders[folder]->items;
		ContactItem *p = (ContactItem *) items[item];
		IcqContact *c = p->contact;

		if (move) {
			items.erase(items.begin() + item);
			addContact(folder, c);
			delete p;
		} else {
			p->icon = getApp()->getLargeFace(c->face, c->status);
			p->text = c->nick.c_str();
		}

		if (folder == selFolder)
			repaintInsideRect();
	}
}

void MyICQCtrl::addContact(int folder, IcqContact *c)
{
	if (folder < 0 || folder >= getFolderCount())
		return;

	ContactItem *p = new ContactItem(c);
	vector<OutBarItem *> &items = folders[folder]->items;

	// Binary insert
	int low = 0, high = items.size() - 1;
	while (low <= high) {
		int mid = (low + high) / 2;
		if (p->compare((ContactItem *) items[mid]) > 0)
			low = mid + 1;
		else
			high = mid - 1;
	}
	insertItem(folder, low, p);
}

void MyICQCtrl::removeContact(QID &qid)
{
	int folder, item;
	if (findItem(qid, folder, item))
		removeItem(folder, item);
}

/*
 * Find an item's position by qid
 */
BOOL MyICQCtrl::findItem(QID &qid, int &folder, int &item)
{
	int nrFolders = folders.size();
	for (int i = 0; i < nrFolders; i++) {
		vector<OutBarItem *> &items = folders[i]->items;
		int nrItems = items.size();

		for (int j = 0; j < nrItems; j++) {
			ContactItem *p = (ContactItem *) items[j];
			if (p->contact->qid == qid) {
				folder = i;
				item = j;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void MyICQCtrl::addGroup(IcqGroup *g)
{
	int n = folders.size() - 3;
	if (n >= 0) {
		GroupItem *p = new GroupItem(g);
		insertItem(n, 0, p);

		if (selFolder == n)
			repaintInsideRect();
	}
}

void MyICQCtrl::removeGroup(uint32 id)
{
	int folder, item;
	if (findGroup(id, folder, item))
		removeItem(folder, item);
}

BOOL MyICQCtrl::findGroup(uint32 id, int &folder, int &item)
{
	int n = folders.size() - 3;
	if (n < 0)
		return FALSE;

	folder = n;
	vector<OutBarItem *> &items = folders[n]->items;
	n = items.size();

	for (int i = 0; i < n; i++) {
		GroupItem *p = (GroupItem *) items[i];
		if (p->group->id == id) {
			item = i;
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Blink text when a user is online
 */
void MyICQCtrl::doBlinkText()
{
	CClientDC dc(this);
	CRect rc;
	getInsideRect(rc);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rc);
	dc.SelectClipRgn(&rgn);
	
	BlinkList::iterator it, next;
	for (it = blinkTextList.begin(); it != blinkTextList.end(); it = next) {
		next = it;
		++next;

		BlinkInfo *p = &(*it);
		int folder, item;
		if (findItem(p->qid, folder, item)) {
			p->frame--;
			if (folder != selFolder || item > getMaxVisibleItem())
				continue;
			if (p->frame >= 0) {
				drawItemText(&dc, item, blinkColors[p->frame % NR_BLINK_COLORS]);
				continue;
			}
			drawItemText(&dc, item, fontColor);
		}
		blinkTextList.erase(it);
	}
	if (blinkTextList.empty())
		KillTimer(IDT_BLINK_TEXT);
}

/*
 * Blink icon when new message comes
 */
void MyICQCtrl::doBlinkImage()
{
	if (blinkImageList.empty()) {
		KillTimer(IDT_BLINK_IMAGE);
		return;
	}

	CClientDC dc(this);
	CRect rc;
	getInsideRect(rc);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rc);

	BlinkList::iterator it;
	for (it = blinkImageList.begin(); it != blinkImageList.end(); ++it) {
		BlinkInfo *p = &(*it);
		int folder, item;
		bool b;

		if (!p->id)
			b = findItem(p->qid, folder, item);
		else
			b = findGroup(p->id, folder, item);
		if (!b)
			continue;

		p->frame++;

		// Draw small icon on the folder
		getFolderRect(rc, folder);
		rc.left += 2;
		rc.right = rc.left + SMALL_ICON_W;
		rc.top += (FOLDER_HEIGHT - SMALL_ICON_H) / 2;
		rc.bottom = rc.top + SMALL_ICON_H;
		drawFolder(&dc, folder, FALSE, FALSE);
		if (p->frame & 1) {
			HICON icon = folders[folder]->items[item]->icon;
			DrawIconEx(dc, rc.left, rc.top, icon, SMALL_ICON_W, SMALL_ICON_H,
				0, NULL, DI_NORMAL);
		}

		if (p->frame >= NR_BLINK_OFFSETS)
			p->frame = 0;
		if (folder != selFolder || item > getMaxVisibleItem())
			continue;

		dc.SelectClipRgn(&rgn);

		CRect rcIcon;
		getIconRect(rc, item);
		rcIcon = rc;
		rcIcon.InflateRect(BLINK_OFFSET, BLINK_OFFSET);
		drawBackground(&dc, rcIcon);
		CPoint &pt = blinkOffsets[p->frame];
		rc.OffsetRect(pt.x, pt.y);
		drawItemImage(&dc, item, rc);

		dc.SelectClipRgn(NULL);
	}
}

void MyICQCtrl::blinkText(QID &qid)
{
	int folder, item;
	if (!findItem(qid, folder, item))
		return;
		
	SetTimer(IDT_BLINK_TEXT, 300, NULL);
	blinkTextList.push_back(BlinkInfo(qid, 5 * NR_BLINK_COLORS));
}

void MyICQCtrl::blinkImage(QID &qid, BOOL blink)
{
	if (blink) {
		int folder, item;
		if (!findItem(qid, folder, item))
			return;

		SetTimer(IDT_BLINK_IMAGE, 400, NULL);
		blinkImageList.push_back(BlinkInfo(qid, 0));
	} else {
		BlinkList::iterator it = blinkImageList.begin();
		while (it != blinkImageList.end()) {
			if ((*it).qid == qid)
				it = blinkImageList.erase(it);
			else
				++it;
		}
		if (blinkImageList.empty())
			KillTimer(IDT_BLINK_IMAGE);
		Invalidate();
	}
}

void MyICQCtrl::blinkImage(uint32 id, BOOL blink)
{
	if (blink) {
		int folder, item;
		if (!findGroup(id, folder, item))
			return;

		SetTimer(IDT_BLINK_IMAGE, 400, NULL);
		blinkImageList.push_back(BlinkInfo(id));
	} else {
		BlinkList::iterator it = blinkImageList.begin();
		while (it != blinkImageList.end()) {
			if ((*it).id == id)
				it = blinkImageList.erase(it);
			else
				++it;
		}
		if (blinkImageList.empty())
			KillTimer(IDT_BLINK_IMAGE);
		Invalidate();
	}
}

/*
 * Serialize state
 */
void MyICQCtrl::save(DBOutStream &out)
{
	out << bgColor << fontColor;
	out << (uint8) largeIconView;

	uint16 nrFolders = folders.size();
	out << nrFolders;
	for (int i = 0; i < nrFolders; i++) {
		OutBarFolder *f = folders[i];
		out << f->text;

		uint16 nrItems = 0;
		if (i != nrFolders - 3)
			nrItems = f->items.size();

		out << nrItems;
		for (int j = 0; j < nrItems; j++)
			out << ((ContactItem *) f->items[j])->contact->qid;
	}
}

void MyICQCtrl::load(DBInStream &in)
{
	int i;
	for (i = folders.size() - 1; i >= 0; i--)
		delete folders[i];
	folders.clear();

	in >> bgColor >> fontColor;
	uint8 b;
	in >> b;
	largeIconView = b;

	uint16 nrFolders;
	in >> nrFolders;
	for (i = 0; i < nrFolders; i++) {
		string name;
		in >> name;
		addFolder(name.c_str());

		uint16 nrItems;
		in >> nrItems;
		for (int j = 0; j < nrItems; j++) {
			QID qid;
			in >> qid;
			IcqContact *c = icqLink->findContact(qid);
			if (c) {
				c->type = (i < nrFolders - 1 ? CONTACT_FRIEND : CONTACT_IGNORE);
				addContact(i, c);
			}
		}
	}
}

int MyICQCtrl::getMaxItem()
{
	int max = OutBarCtrl::getMaxItem();

	if (selFolder != folders.size() - 3 && onlineOnly) {
		while (--max >= 0) {
			if (contact(max)->status != STATUS_OFFLINE) {
				++max;
				break;
			}
		}
	}
	return max;
}

void MyICQCtrl::drawBackground(CDC *pDC, CRect &rc)
{
	if (!skin) {
		OutBarCtrl::drawBackground(pDC, rc);
		return;
	}

	// Tile the bitmap on the whole background
	CBitmap &back = skin->bitmaps[SKIN_BACK];
	BITMAP bm;
	back.GetBitmap(&bm);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap *oldBitmap = memDC.SelectObject(&back);

	for (int y = rc.top; y < rc.bottom; y += bm.bmHeight) {
		int h = bm.bmHeight;
		if (h > rc.bottom - y)
			h = rc.bottom - y;
		for (int x = rc.left; x < rc.right; x += bm.bmWidth) {
			int w = bm.bmWidth;
			if (w > rc.right - x)
				w = rc.right - x;
			pDC->BitBlt(x, y, w, h, &memDC, 0, 0, SRCCOPY);
		}
	}

	memDC.SelectObject(oldBitmap);
}

void MyICQCtrl::drawItemText(CDC *pDC, int i, COLORREF color)
{
	// Show nick name of contacts that are in away status in a different color
	IcqContact *c = contact(i);
	if (c && c->status == STATUS_AWAY)
		color = RGB(0, 0, 255);

	OutBarCtrl::drawItemText(pDC, i, color);
}

void MyICQCtrl::drawFolder(CDC *pDC, int i, BOOL pressed, BOOL hilight)
{
	if (!skin) {
		OutBarCtrl::drawFolder(pDC, i, pressed, hilight);
		return;
	}
	
	if (pressed)
		hilight = FALSE;
	CBitmap *bm = &skin->bitmaps[hilight ? SKIN_FOLDER_LIGHT : SKIN_FOLDER];

	CRect rc;
	getFolderRect(rc, i);
	int l = rc.left;
	int r = rc.right;
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap *oldBitmap = memDC.SelectObject(bm);
	BITMAP info;
	bm->GetBitmap(&info);

	pDC->BitBlt(l, rc.top, 40, info.bmHeight, &memDC, 0, 0, SRCCOPY);
	l += 40;
	r -= 40;
	pDC->BitBlt(r, rc.top, 40, info.bmHeight, &memDC, info.bmWidth - 40, 0, SRCCOPY);

	info.bmWidth -= 40 * 2;
	while (l < r) {
		int w = info.bmWidth;
		if (w > r - l)
			w = r - l;
		pDC->BitBlt(l, rc.top, w, info.bmHeight, &memDC, 40, 0, SRCCOPY);
		l += info.bmWidth;
	}

	memDC.SelectObject(oldBitmap);

	drawFolderText(pDC, i, rc);
}

void MyICQCtrl::drawScroll(CDC *pDC, int i, BOOL pressed)
{
	if (!skin) {
		OutBarCtrl::drawScroll(pDC, i, pressed);
		return;
	}

	CBitmap *scroll;
	if (i == SCROLL_DIR_UP)
		scroll = &skin->bitmaps[pressed ? SKIN_SCROLLUP_PRESSED : SKIN_SCROLLUP];
	else
		scroll = &skin->bitmaps[pressed ? SKIN_SCROLLDOWN_PRESSED : SKIN_SCROLLDOWN];

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap *oldBitmap = memDC.SelectObject(scroll);
	CRect rc;
	getScrollRect(rc, i);

	BITMAP bm;
	scroll->GetBitmap(&bm);
	pDC->BitBlt(rc.left, rc.top, bm.bmWidth, bm.bmHeight, &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(oldBitmap);
}

BEGIN_MESSAGE_MAP(MyICQCtrl, OutBarCtrl)
	//{{AFX_MSG_MAP(MyICQCtrl)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// MyICQCtrl message handlers

void MyICQCtrl::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_BLINK_TEXT)
		doBlinkText();
	else if (nIDEvent == IDT_BLINK_IMAGE)
		doBlinkImage();
	else
		OutBarCtrl::OnTimer(nIDEvent);
}
