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

// MsgTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "MsgTreeView.h"
#include "MsgListView.h"
#include "myicqDlg.h"
#include "serversession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void writeMsgTxt(CStdioFile &file, uint32 uin)
{
	CString tmp, nick, str;
	tmp.LoadString(IDS_MSG_OBJ);

	if (uin) {
		IcqContact *c = icqLink->findContact(uin);
		if (c)
			nick = c->nick.c_str();
		else
			nick.Format("%lu", uin);

		str.Format("%s:%lu(%s)\n", (LPCTSTR) tmp, uin, (LPCTSTR) nick);
	} else {
		str.LoadString(IDS_SYSMSG);
		str = tmp + ":" + str + '\n';
	}

	file.WriteString("--------------------------------------------------\n");
	file.WriteString(str);
	file.WriteString("--------------------------------------------------\n");

	PtrList msgList;
	IcqDB::loadMsg(uin, msgList);
	while (!msgList.empty()) {
		IcqMsg *msg = (IcqMsg *) msgList.front();
		msgList.pop_front();

		if (!uin)
			nick.Format("%lu", msg->uin);
		exportMsgTxt(file, msg, nick);
		delete msg;
	}

	file.WriteString("\n");
}

void writeContact(CStdioFile &file, IcqContact *c) {
	FILE *fp = file.m_pStream;
	CString str, tmp;
	tmp.LoadString(IDS_MSG_OBJ);
	str.Format("%s:%lu(%s)\n", (LPCTSTR) tmp, c->uin, c->nick.c_str());

	file.WriteString("--------------------------------------------------\n");
	file.WriteString(str);
	file.WriteString("--------------------------------------------------\n");

	str.LoadString(IDS_UIN);
	tmp.Format("%lu", c->uin);
	file.WriteString(str + " " + tmp + "\n\n");
	str.LoadString(IDS_NICK);
	file.WriteString(str + " " + c->nick.c_str() + "\n\n");
	str.LoadString(IDS_AGE);
	tmp.Format("%d", c->age);
	file.WriteString(str + " " + tmp + "\n\n");
	str.LoadString(IDS_GENDER);
	file.WriteString(str + " " + getApp()->genderNames[c->gender] + "\n\n");
	str.LoadString(IDS_COUNTRY);
	file.WriteString(str + " " + c->country.c_str() + "\n\n");
	str.LoadString(IDS_PROVINCE);
	file.WriteString(str + " " + c->province.c_str() + "\n\n");
	str.LoadString(IDS_CITY);
	file.WriteString(str + " " + c->city.c_str() + "\n\n");

	str.LoadString(IDS_EMAIL);
	file.WriteString(str + " " + c->email.c_str() + "\n\n");
	str.LoadString(IDS_ADDRESS);
	file.WriteString(str + " " + c->address.c_str() + "\n\n");
	str.LoadString(IDS_ZIPCODE);
	file.WriteString(str + " " + c->zipcode.c_str() + "\n\n");
	str.LoadString(IDS_TEL);
	file.WriteString(str + " " + c->tel.c_str() + "\n\n");

	str.LoadString(IDS_REALNAME);
	file.WriteString(str + " " + c->name.c_str() + "\n\n");
	str.LoadString(IDS_BLOODTYPE);
	file.WriteString(str + " " + getApp()->bloodNames[c->blood] + "\n\n");
	str.LoadString(IDS_COLLEGE);
	file.WriteString(str + " " + c->college.c_str() + "\n\n");
	str.LoadString(IDS_PROFESSION);
	file.WriteString(str + " " + c->profession.c_str() + "\n\n");
	str.LoadString(IDS_HOMEPAGE);
	file.WriteString(str + " " + c->homepage.c_str() + "\n\n");
	str.LoadString(IDS_SELFINTRO);
	file.WriteString(str + " " + c->intro.c_str() + "\n\n");

	file.WriteString("\n");
}


#define NODE_FOLDER		0xffffffff

/////////////////////////////////////////////////////////////////////////////
// CMsgTreeView

IMPLEMENT_DYNCREATE(CMsgTreeView, CTreeView)

CMsgTreeView::CMsgTreeView()
{
	listView = NULL;
}

CMsgTreeView::~CMsgTreeView()
{
}

DWORD CMsgTreeView::getUIN()
{
	HTREEITEM item = GetTreeCtrl().GetSelectedItem();
	return (item ? GetTreeCtrl().GetItemData(item) : NODE_FOLDER);
}

void CMsgTreeView::save(DBOutStream &out)
{
	out << GetTreeCtrl().GetTextColor();
	out << GetTreeCtrl().GetBkColor();
}

void CMsgTreeView::load(DBInStream &in)
{
	COLORREF color;
	in >> color;
	GetTreeCtrl().SetTextColor(color);
	in >> color;
	GetTreeCtrl().SetBkColor(color);
}

HTREEITEM CMsgTreeView::findItem(HTREEITEM item, DWORD uin)
{
	CTreeCtrl &tree = GetTreeCtrl();
	if (tree.ItemHasChildren(item)) {
		HTREEITEM child = tree.GetChildItem(item);
		while (child) {
			HTREEITEM ret = findItem(child, uin);
			if (ret)
				return ret;
			child = tree.GetNextSiblingItem(child);
		}
	} else if (tree.GetItemData(item) == uin)
		return item;

	return NULL;
}

void CMsgTreeView::expand(DWORD uin)
{
	CTreeCtrl &tree = GetTreeCtrl();
	HTREEITEM root = tree.GetRootItem();

	if (uin) {
		HTREEITEM item = findItem(root, uin);
		if (item) {
			tree.SelectItem(item);
			while ((item = tree.GetParentItem(item)) != NULL)
				tree.Expand(item, TVE_EXPAND);
		}
	} else
		tree.Expand(root, TVE_EXPAND);
}

void CMsgTreeView::delMsg(HTREEITEM item)
{
	CTreeCtrl &tree = GetTreeCtrl();
	uint32 uin = tree.GetItemData(item);
	if (uin == NODE_FOLDER) {
		HTREEITEM child = tree.GetChildItem(item);
		while (child) {
			delMsg(child);
			child = tree.GetNextSiblingItem(child);
		}
	} else {
		uin = tree.GetItemData(item);
		IcqDB::delMsg(uin);
	}
}

void CMsgTreeView::exportTxt(CStdioFile &file, HTREEITEM item)
{
	CTreeCtrl &tree = GetTreeCtrl();
	uint32 uin = tree.GetItemData(item);

	if (uin == NODE_FOLDER) {
		HTREEITEM child = tree.GetChildItem(item);
		while (child) {
			exportTxt(file, child);
			child = tree.GetNextSiblingItem(child);
		}
	} else
		writeMsgTxt(file, uin);
}

void CMsgTreeView::exportContact(CStdioFile &file, HTREEITEM item)
{
	CTreeCtrl &tree = GetTreeCtrl();
	uint32 uin = tree.GetItemData(item);

	if (uin == NODE_FOLDER) {
		HTREEITEM child = tree.GetChildItem(item);
		while (child) {
			exportContact(file, child);
			child = tree.GetNextSiblingItem(child);
		}
	} else if (uin) {
		CString nick;
		IcqContact *c = icqLink->findContact(uin);
		if (c)
			writeContact(file, c);
	}
}

void CMsgTreeView::exportBak(const char *fileName, HTREEITEM item)
{
	CTreeCtrl &tree = GetTreeCtrl();
	uint32 uin = tree.GetItemData(item);

	if (uin == NODE_FOLDER) {
		HTREEITEM child = tree.GetChildItem(item);
		while (child) {
			exportBak(fileName, child);
			child = tree.GetNextSiblingItem(child);
		}
	} else
		IcqDB::exportMsg(fileName, uin);
}

void CMsgTreeView::exportTxt(CStdioFile &file)
{
	HTREEITEM item = GetTreeCtrl().GetSelectedItem();
	if (item)
		exportTxt(file, item);
}

void CMsgTreeView::exportContact(CStdioFile &file)
{
	HTREEITEM item = GetTreeCtrl().GetSelectedItem();
	if (item)
		exportContact(file, item);
}

void CMsgTreeView::delSelectedMsg()
{
	CTreeCtrl &tree = GetTreeCtrl();
	HTREEITEM item = tree.GetSelectedItem();
	if (item) {
		delMsg(item);
		listView->clear();
	}
}

BEGIN_MESSAGE_MAP(CMsgTreeView, CTreeView)
	//{{AFX_MSG_MAP(CMsgTreeView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DEL, OnUpdateEditDel)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_CONTACT, OnUpdateExportContact)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_BAK, OnUpdateExportBak)
	ON_COMMAND(ID_EXPORT_BAK, OnExportBak)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_EXPORT_BACKUP, OnExportBackup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgTreeView diagnostics

#ifdef _DEBUG
void CMsgTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CMsgTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMsgTreeView message handlers

BOOL CMsgTreeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_HASLINES;
	return CTreeView::PreCreateWindow(cs);
}

int CMsgTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CTreeCtrl &tree = GetTreeCtrl();
	tree.SetImageList(&getApp()->smallImageList, TVSIL_NORMAL);
	tree.SetItemHeight(24);

	IcqUser &myInfo = icqLink->myInfo;
	CString str;
	str.Format("%lu(%s)", myInfo.uin, myInfo.nick.c_str());
	HTREEITEM root = tree.InsertItem(str, getApp()->getImageIndex(myInfo.face),
		getApp()->getImageIndex(myInfo.face, STATUS_AWAY), NULL);
	tree.SetItemData(root, NODE_FOLDER);

	MyICQCtrl &outbar = ((CIcqDlg *) AfxGetMainWnd())->outbarCtrl;
	int n = outbar.getFolderCount();
	CString name;
	int image;
	HTREEITEM stranger = NULL;

	UinList msgUinList;
	IcqDB::getMsgUinList(msgUinList);

	for (int i = 0; i < n; ++i) {
		outbar.getFolderName(i, name);
		image = getApp()->iconIndex(ICON_FOLDER);
		HTREEITEM parent = tree.InsertItem(name, image, image, root);
		tree.SetItemData(parent, NODE_FOLDER);

		int nrItems = outbar.getItemCount(i);
		for (int j = 0; j < nrItems; ++j) {
			IcqContact *c = outbar.contact(i, j);
			str.Format("%lu(%s)", c->uin, c->nick.c_str());
			HTREEITEM hItem = tree.InsertItem(str, getApp()->getImageIndex(c->face),
				getApp()->getImageIndex(c->face, STATUS_AWAY), parent);
			tree.SetItemData(hItem, c->uin);

			msgUinList.remove(c->uin);
		}

		if (i == n - 2)
			stranger = parent;
	}

	// add the remaining UINs to the stranger folder
	UinList::iterator it;
	for (it = msgUinList.begin(); it != msgUinList.end(); ++it) {
		uint32 uin = *it;
		if (uin) {
			str.Format("%lu", uin);
			HTREEITEM item = tree.InsertItem(str, getApp()->getImageIndex(0),
				getApp()->getImageIndex(0, STATUS_AWAY), stranger);
			tree.SetItemData(item, uin);
		}
	}

	str.LoadString(IDS_SYSMSG);
	image = getApp()->iconIndex(ICON_SYSMSG);
	tree.InsertItem(str, image, image, root);
	
	return 0;
}

void CMsgTreeView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMenu menu;
	menu.LoadMenu(IDR_MSGMGR_POPUP);
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	UINT flags;
	HTREEITEM item = GetTreeCtrl().HitTest(pt, &flags);
	GetTreeCtrl().SelectItem(item);

	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, GetParentFrame());

	*pResult = 0;
}

void CMsgTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM item = pNMTreeView->itemNew.hItem;
	uint32 uin = (item ? GetTreeCtrl().GetItemData(item) : NODE_FOLDER);
	listView->showMsg(uin);

	*pResult = 0;
}

void CMsgTreeView::OnUpdateEditDel(CCmdUI* pCmdUI) 
{
	if (GetParentFrame()->GetActiveView() == this)
		pCmdUI->Enable(GetTreeCtrl().GetSelectedItem() != NULL);
}

void CMsgTreeView::OnUpdateExportContact(CCmdUI* pCmdUI) 
{
	if (GetParentFrame()->GetActiveView() == this) {
		CTreeCtrl &tree = GetTreeCtrl();
		HTREEITEM item = tree.GetSelectedItem();
		if (item) {
			uint32 uin = tree.GetItemData(item);
			pCmdUI->Enable(uin != 0);
		}
	}
}

void CMsgTreeView::OnUpdateExportBak(CCmdUI* pCmdUI) 
{
	if (GetParentFrame()->GetActiveView() == this)
		pCmdUI->Enable(GetTreeCtrl().GetSelectedItem() != NULL);
}

void CMsgTreeView::OnExportBak() 
{
	CTreeCtrl &tree = GetTreeCtrl();
	HTREEITEM item = tree.GetSelectedItem();
	uint32 uin = tree.GetItemData(item);

	CString fileName;
	if (uin > 0 && uin <= 0xfffffff0)
		fileName.Format("%lu", uin);
	CString filter;
	filter.LoadString(IDS_FILTER_BAK);
	CFileDialog dlg(FALSE, "bak", fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (dlg.DoModal() == IDOK)
		exportBak(dlg.GetPathName(), item);
}

void CMsgTreeView::OnFileImport() 
{
	CString filter;
	filter.LoadString(IDS_FILTER_BAK);
	CFileDialog dlg(TRUE, "bak", NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK) {
		IcqDB::importRecord(dlg.GetPathName());

		// refresh
		CTreeCtrl &tree = GetTreeCtrl();
		HTREEITEM item = tree.GetSelectedItem();
		uint32 uin = (item ? tree.GetItemData(item) : NODE_FOLDER);
		listView->showMsg(uin);
	}
}

void CMsgTreeView::OnExportBackup() 
{
	CString filter;
	filter.LoadString(IDS_FILTER_BAK);
	CFileDialog dlg(FALSE, "bak", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (dlg.DoModal() == IDOK) {
		CString pathName = dlg.GetPathName();
		CFile::Remove(pathName);
		exportBak(pathName, GetTreeCtrl().GetRootItem());
	}
}
