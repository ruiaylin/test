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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class ContactItem : public OutBarItem {
public:
	ContactItem(IcqContact *c)
		:OutBarItem(c->nick.c_str(), getApp()->getImageIndex(c->face, c->status)),
		contact(c) {}
	int compare(ContactItem *item);
	
	IcqContact *contact;
};

int ContactItem::compare(ContactItem *item)
{
	BOOL b1 = (contact->status != STATUS_OFFLINE);
	BOOL b2 = (item->contact->status != STATUS_OFFLINE);
	if (b1 != b2)
		return (b1 ? -1 : 1);

	int ret = text.Compare(item->text);
	if (ret != 0)
		return ret;

	return (contact->uin > item->contact->uin ? 1 : -1);
}


static COLORREF blinkColors[] = {
	RGB(255, 0, 0),
	RGB(255, 255, 0),
	RGB(0, 255, 255),
	RGB(0, 255, 0),
	RGB(0, 0, 255),
};
#define NR_BLINK_COLORS	(sizeof(blinkColors) / sizeof(*blinkColors))

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

IcqContact *MyICQCtrl::contact(int folder, int item)
{
	return ((ContactItem *) folders[folder]->items[item])->contact;
}

void MyICQCtrl::update(uint32 uin, BOOL move)
{
	int folder, item;
	if (findItem(uin, folder, item)) {
		vector<OutBarItem *> &items = folders[folder]->items;
		ContactItem *p = (ContactItem *) items[item];
		IcqContact *c = p->contact;
		
		if (move) {
			items.erase(items.begin() + item);
			addContact(folder, c);
			delete p;
		} else {
			p->image = getApp()->getImageIndex(c->face, c->status);
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

void MyICQCtrl::removeContact(uint32 uin)
{
	int folder, item;
	if (findItem(uin, folder, item)) {
		vector<OutBarItem *> &items = folders[folder]->items;
		items.erase(items.begin() + item);
		if (selFolder == folder)
			repaintInsideRect();
	}
}

BOOL MyICQCtrl::findItem(uint32 uin, int &folder, int &item)
{
	for (int i = 0; i < (int) folders.size(); i++) {
		vector<OutBarItem *> &items = folders[i]->items;
		for (int j = 0; j < (int) items.size(); j++) {
			if (((ContactItem *)items[j])->contact->uin == uin) {
				folder = i;
				item = j;
				return TRUE;
			}
		}
	}
	return FALSE;
}

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
		if (findItem(p->uin, folder, item)) {
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

void MyICQCtrl::doBlinkImage()
{
	if (blinkImageList.empty()) {
		KillTimer(IDT_BLINK_IMAGE);
		return;
	}
	BlinkInfo *p = &blinkImageList.back();
	int folder, item;
	if (!findItem(p->uin, folder, item)) {
		blinkImageList.pop_back();
		return;
	}
	int img = folders[folder]->items[item]->image;
	
	CClientDC dc(this);
	CRect rc;
	getFolderRect(rc, folder);
	rc.left += 2;
	rc.right = rc.left + SMALL_ICON_W;
	rc.top += (FOLDER_HEIGHT - SMALL_ICON_H) / 2;
	rc.bottom = rc.top + SMALL_ICON_H;
	if (p->frame & 1)
		drawFolder(&dc, folder, FALSE, FALSE);
	else
		smallIcons->Draw(&dc, img, rc.TopLeft(), ILD_TRANSPARENT);
	
	getInsideRect(rc);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rc);
	dc.SelectClipRgn(&rgn);

	BlinkList::iterator it;
	for (it = blinkImageList.begin(); it != blinkImageList.end(); ++it) {
		p = &(*it);
		if (!findItem(p->uin, folder, item))
			continue;

		p->frame++;
		if (p->frame >= (int) NR_BLINK_OFFSETS)
			p->frame = 0;
		if (folder != selFolder || item > getMaxVisibleItem())
			continue;

		CRect rcIcon;
		getIconRect(rc, item);
		rcIcon = rc;
		rcIcon.InflateRect(BLINK_OFFSET, BLINK_OFFSET);
		drawBackground(&dc, rcIcon);
		CPoint &pt = blinkOffsets[p->frame];
		rc.OffsetRect(pt.x, pt.y);
		drawItemImage(&dc, item, rc);
	}
}

void MyICQCtrl::blinkText(uint32 uin)
{
	int folder, item;
	if (!findItem(uin, folder, item))
		return;
		
	SetTimer(IDT_BLINK_TEXT, 300, NULL);
	blinkTextList.push_back(BlinkInfo(uin, 5 * NR_BLINK_COLORS));
}

void MyICQCtrl::blinkImage(uint32 uin, BOOL blink)
{
	if (blink) {
		int folder, item;
		if (!findItem(uin, folder, item))
			return;
		
		SetTimer(IDT_BLINK_IMAGE, 400, NULL);
		blinkImageList.push_back(BlinkInfo(uin, 0));
	} else {
		BlinkList::iterator it = blinkImageList.begin();
		while (it != blinkImageList.end()) {
			if ((*it).uin == uin)
				it = blinkImageList.erase(it);
			else
				++it;
		}
		if (blinkImageList.empty())
			KillTimer(IDT_BLINK_IMAGE);
		Invalidate();
	}
}

void MyICQCtrl::save(DBOutStream &out)
{
	out << bgColor << fontColor;
	out << (uint8) largeIconView;
	
	uint16 nrFolders = folders.size();
	out << nrFolders;
	for (int i = 0; i < nrFolders; i++) {
		OutBarFolder *f = folders[i];
		out << f->text;
		
		uint16 nrItems = f->items.size();
		out << nrItems;
		for (int j = 0; j < nrItems; j++)
			out << ((ContactItem *) f->items[j])->contact->uin;
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
			uint32 uin;
			in >> uin;
			IcqContact *c = icqLink->findContact(uin);
			if (c) {
				c->type = (i < nrFolders - 1 ? CONTACT_FRIEND : CONTACT_IGNORE);
				addContact(i, c);
			}
		}
	}
}

int MyICQCtrl::getMaxVisibleItem()
{
	int max = OutBarCtrl::getMaxVisibleItem();
	if (onlineOnly) {
		for (; max >= 0; --max) {
			if (contact(max)->status != STATUS_OFFLINE)
				break;
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
	OutBarCtrl::drawItemText(pDC, i,
		contact(i)->status == STATUS_AWAY ? RGB(0, 0, 255) : color);
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
