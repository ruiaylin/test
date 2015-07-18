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

// myicqDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "myicqDlg.h"
#include <mmsystem.h>
#include "serversession.h"
#include "tcpsession.h"
#include "icqwindow.h"
#include "icqdb.h"
#include "icqconfig.h"
#include "icqplugin.h"

#include "GfxPopupMenu.h"
#include "LoginDlg.h"
#include "RegWizard.h"
#include "ViewDetailDlg.h"
#include "SendMsgDlg.h"
#include "SearchWizard.h"
#include "SysMsgDlg.h"
#include "ViewMsgDlg.h"
#include "SysHistoryDlg.h"
#include "DelFriendDlg.h"
#include "ModifyDetailDlg.h"
#include "SysOptionDlg.h"
#include "LoginAgainDlg.h"
#include "NotifyWnd.h"
#include "AboutDlg.h"
#include "MsgMgrWnd.h"
#include "SendRequestDlg.h"
#include "BroadcastDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_GFX_SHOWONLINE		50001
#define ID_GFX_RENAME			50002
#define ID_GFX_REMOVE			50003
#define ID_STATUS_AWAY			40000
#define ID_PLUGIN				60000
#define ID_SKIN_FIRST			70000

#define IDC_OUTBAR				1001
#define ID_HOTKEY				1002
#define ID_TOOLTIP				1003
#define ID_NOTIFYICON			1004

#define IDT_RESEND				1001
#define IDT_KEEPALIVE			1002
#define IDT_STATUS				1003
#define IDT_BLINK				1004
#define IDT_DBLCLK				1005
#define IDT_HOVER				1006
#define IDT_AUTO_SWITCH_STATUS	1007

#define WM_HOSTFOUND			(WM_USER + 101)
#define WM_NOTIFYICON			(WM_USER + 102)

#define HOTKEY_DEFAULT	MAKELONG(0x5a, MOD_CONTROL | MOD_ALT)

enum {
	ALIGN_NONE,
	ALIGN_TOP,
	ALIGN_LEFT,
	ALIGN_RIGHT
};

/////////////////////////////////////////////////////////////////////////////
// CIcqDlg dialog

CIcqDlg::CIcqDlg(CWnd* pParent /*=NULL*/)
: CCaptionBtnDlg(2, CIcqDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIcqDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	iconBlank = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BLANK),
		IMAGE_ICON, 0, 0, 0);
	iconSysMsg = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SYSMSG),
		IMAGE_ICON, 0, 0, 0);

	regMode = -1;
	alignType = ALIGN_NONE;
	isHide = FALSE;
	currentMenu = NULL;
	curContact = NULL;

	outbarCtrl.setListener(this);
	outbarCtrl.setImageList(&getApp()->largeImageList, &getApp()->smallImageList);
}

CIcqDlg::~CIcqDlg()
{
}

void *CIcqDlg::getFaceIcon(int face, int status)
{
	int i = getApp()->getImageIndex(face, status);
	return (void *) getApp()->largeImageList.ExtractIcon(i);
}

void CIcqDlg::popupMsg(void *icon, const char *text, uint32 t)
{
	new CNotifyWnd((HICON) icon, text, t);
}

void CIcqDlg::drawCaptionBtn(CDC *pDC, CRect &rc, int button, BOOL pressed)
{
	ASSERT(button < 2);

	if (!pressed) {
		pDC->Draw3dRect(rc, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
		rc.InflateRect(-1, -1);
		pDC->Draw3dRect(rc, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_3DSHADOW));
	} else {
		pDC->Draw3dRect(rc, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_BTNHILIGHT));
		rc.InflateRect(-1, -1);
		pDC->Draw3dRect(rc, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_BTNFACE));
	}

	rc.InflateRect(-1, -1);
	pDC->FillSolidRect(rc, GetSysColor(COLOR_BTNFACE));

	CPen pen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
	CPen *penOld = pDC->SelectObject(&pen);
	int x, y;

	x = rc.left;
	y = (rc.top + rc.bottom) / 2;
	if (pressed) {
		x++;
		y++;
	}
	pDC->MoveTo(x, y);
	pDC->LineTo(x + rc.Width() - 2, y);

	if (button == 1) {
		x = (rc.left + rc.right) / 2 - 1;
		y = rc.top + 1;
		if (pressed) {
			x++;
			y++;
		}
		pDC->MoveTo(x, y);
		pDC->LineTo(x, y + rc.Height() - 2);
	}

	pDC->SelectObject(penOld);
}

void CIcqDlg::onCaptionBtnClicked(int button)
{
	ASSERT(button < 2);

	if (button == 0) {
		ShowWindow(SW_MINIMIZE);
		ShowWindow(SW_HIDE);
	} else if (button == 1) {
		CGfxPopupMenu menu;
		menu.CreatePopupMenu();

		int id = 1000;
		menu.appendMenu(id, options.skin.empty() ? IDB_CHECK : IDB_FOLDER, IDS_SKIN_RESTORE);

		CFileFind finder;
		BOOL cont = finder.FindFile(getApp()->rootDir + "skins\\*");
		while (cont) {
			cont = finder.FindNextFile();
			if (!finder.IsDots() && finder.IsDirectory()) {
				int bmpID = (finder.GetFileTitle() == options.skin.c_str() ? IDB_CHECK : IDB_FOLDER);
				menu.appendMenu(++id, bmpID, finder.GetFileTitle());
			}
		}

		CPoint pt;
		GetCursorPos(&pt);
		id = menu.TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this);
		if (id == 0)
			return;

		ShowWindow(SW_MINIMIZE);

		if (id == 1000) {
			outbarCtrl.setSkin(NULL);
			options.skin = "";
		} else {
			CString str;
			menu.GetMenuString(id, str, 0);
			if (skin.load(getApp()->rootDir + "skins\\" + str))
				outbarCtrl.setSkin(&skin);

			options.skin = str;
		}

		IcqDB::saveOptions(options);
		ShowWindow(SW_NORMAL);
	}
}

void CIcqDlg::onSendError(uint32 seq)
{
	if (!seq) {
		// Destination port is unreachable.
		IcqWindow *win = findWindow(WIN_REG_WIZARD);
		if (win) {
			((CRegWizard *) win)->onSendError(seq);
			return;
		}
	} else if (lastSeq != seq) {
		IcqLink::onSendError(seq);
		return;
	}

	lastSeq = 0;
	onError();
}

void CIcqDlg::onConnect(bool success)
{
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win) {
		((CRegWizard *) win)->onConnect(success);
		return;
	}

	if (success)
		lastSeq = serverSession()->login(myInfo.passwd.c_str(), pendingStatus);
	else {
		myInfo.status = STATUS_OFFLINE;
		updateStatusIcon();
		logout();
	}
}

IcqContact *CIcqDlg::addContact(uint32 uin, int folder)
{
	outbarCtrl.removeContact(uin);
	
	IcqContact *c = findContact(uin);
	if (!c) {
		c = new IcqContact;
		c->uin = uin;
		CString nick;
		nick.Format("%lu", uin);
		c->nick = nick;
		contactList.push_back(c);
	}
	
	int n = outbarCtrl.getFolderCount();
	if (folder == n - 1)
		c->type = CONTACT_IGNORE;
	else if (folder == n - 2)
		c->type = CONTACT_STRANGER;
	else
		c->type = CONTACT_FRIEND;
	
	outbarCtrl.addContact(folder, c);
	outbarCtrl.repaintInsideRect();
	saveGroupInfo();
	return c;
}

void CIcqDlg::login(uint32 status)
{
	pendingStatus = status;
	
	SetTimer(IDT_STATUS, 250, NULL);
	m_btnStatus.setText(IDS_STATUS_LOGGING_IN);

	resolveHost();
}

void CIcqDlg::logout()
{
	IcqLink::logout();

	KillTimer(IDT_RESEND);
	KillTimer(IDT_KEEPALIVE);
	KillTimer(IDT_AUTO_SWITCH_STATUS);

	PtrList::iterator it;
	for (it = contactList.begin(); it != contactList.end(); ++it) {
		IcqContact *c = (IcqContact *) *it;
		outbarCtrl.update(c->uin, TRUE);
	}
}

void CIcqDlg::addFriend(IcqContact *c, int folder)
{
	if (folder < 0)
		folder = outbarCtrl.getSelFolder();
	c->type = CONTACT_FRIEND;

	CSysMsgDlg *win = new CSysMsgDlg(c->uin);
	win->Create(IDD_SYS_MESSAGE);
}

BOOL CIcqDlg::delFriend(IcqContact *c, int folder)
{
	CString str;
	str.Format(IDS_PROMPT_DEL_FRIEND, c->uin);
	if (AfxMessageBox(str, MB_YESNO) != IDYES)
		return FALSE;

	{
		CDelFriendDlg dlg(c->uin);
		if (dlg.DoModal() != IDOK)
			return FALSE;
	}
		
	int n = outbarCtrl.getFolderCount();
	if (folder == n - 1 || folder < 0)
		destroyContact(c->uin);
	
	if (folder == n - 2 || folder < 0)
		IcqDB::delContact(c->uin);
	outbarCtrl.removeContact(c->uin);
	if (folder >= 0) {
		outbarCtrl.addContact(folder, c);
		saveGroupInfo();
		c->type = (folder == n - 1 ? CONTACT_IGNORE : CONTACT_STRANGER);
	} else {
		contactList.remove(c);
		delete c;
	}
	return TRUE;
}

void CIcqDlg::onHostFound(in_addr &addr)
{
	if (addr.s_addr != INADDR_NONE)
		serverSession()->connect(inet_ntoa(addr), options.port);
	else
		onError();
}

void CIcqDlg::onlineNotify(IcqContact *c)
{
	if (options.flags.test(UF_ONLINE_NOTIFY)) {
		options.playSound(SOUND_ONLINE, c);
		outbarCtrl.blinkText(c->uin);

		if (c->flags.test(CF_OPTION_POPUP_ONLINE)) {
			CString str;
			str.Format(IDS_NOTIFY_ONLINE, c->nick.c_str(), c->uin);
			new CNotifyWnd(getApp()->largeImageList.ExtractIcon(c->face), str);
		}
	}
}

void CIcqDlg::addNotifyIcon()
{
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = m_hWnd;
	tnid.uID = ID_NOTIFYICON;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = WM_NOTIFYICON;
	tnid.hIcon = getApp()->m_hIcon;
	lstrcpy(tnid.szTip, AfxGetAppName());

	Shell_NotifyIcon(NIM_ADD, &tnid);
}

void CIcqDlg::removeNotifyIcon()
{
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = m_hWnd;
	tnid.uID = ID_NOTIFYICON;

	Shell_NotifyIcon(NIM_DELETE, &tnid);
}

void CIcqDlg::modifyNotifyIcon(HICON hIcon)
{
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = m_hWnd;
	tnid.uID = ID_NOTIFYICON;
	tnid.uFlags = NIF_ICON;
	tnid.hIcon = hIcon;

	Shell_NotifyIcon(NIM_MODIFY, &tnid);
}

void CIcqDlg::popupMainMenu(UINT nFlags, CPoint &pt)
{
	CGfxPopupMenu menu;
	CMenu tmp;
	tmp.LoadMenu(IDR_MAIN);
	menu.Attach(*tmp.GetSubMenu(0));
	CGfxPopupMenu fileMenu;
	fileMenu.Attach(*menu.GetSubMenu(0));

	fileMenu.modifyMenu(ID_MESSAGE_MGR, IDB_MSG_HISTORY);
	fileMenu.modifyMenu(ID_CONTACT_MGR, IDB_CONTACT_MGR);

	menu.modifyMenu(0, IDB_FILEMGR, FALSE);
	menu.modifyMenu(ID_CHANGE_USER, IDB_CHANGE_USER);
	menu.modifyMenu(ID_REG_WIZARD, IDB_REG_WIZARD);
	menu.modifyMenu(ID_MODIFY_INFO, IDB_VIEW_DETAIL);
	menu.modifyMenu(ID_SYS_OPTION, IDB_SYS_OPTION);
	menu.modifyMenu(ID_HELP, IDB_HELP);
	menu.modifyMenu(ID_APP_EXIT, IDB_EXIT);
	if (myInfo.uin >= 1000)
		menu.DeleteMenu(ID_BROADCAST_MSG, 0);
	else
		menu.modifyMenu(ID_BROADCAST_MSG, iconSysMsg);

	menu.setVertBitmap(IDB_VERT);

	currentMenu = &menu;
	menu.TrackPopupMenu(nFlags, pt.x, pt.y, this);
	currentMenu = NULL;
}

void CIcqDlg::popupStatusMenu(UINT nFlags, CPoint &pt)
{
	CMenu tmp;
	tmp.LoadMenu(IDR_STATUS);
	CGfxPopupMenu statusMenu;
	statusMenu.Attach(*tmp.GetSubMenu(0));
	CGfxPopupMenu awayMenu;
	awayMenu.Attach(*statusMenu.GetSubMenu(2));

	statusMenu.modifyMenu(ID_STATUS_ONLINE, IDB_STATUS_ONLINE);
	statusMenu.modifyMenu(2, IDB_STATUS_AWAY, FALSE);
	statusMenu.modifyMenu(ID_STATUS_INVIS, IDB_STATUS_INVIS);
	statusMenu.modifyMenu(ID_STATUS_OFFLINE, IDB_STATUS_OFFLINE);

	bool autoReply = options.flags.test(UF_AUTO_REPLY);
	UINT bitmap;

	StrList l;
	IcqDB::loadAutoReply(l);
	int pos = 0;
	UINT id = ID_STATUS_AWAY;
	StrList::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		bitmap = (autoReply && *it == options.autoReplyText) ?
			IDB_STATUS_AWAY : IDB_STATUS_OFFLINE;
		CString str = (*it).c_str();
		if (str.GetLength() > 20)
			str = str.Left(20) + "...";
		awayMenu.InsertMenu(pos, MF_BYPOSITION, id++, str);
		awayMenu.modifyMenu(pos++, bitmap, FALSE);
		awayMenu.InsertMenu(pos++, MF_SEPARATOR | MF_BYPOSITION);
	}

	bitmap = autoReply ? IDB_STATUS_OFFLINE : IDB_STATUS_AWAY;
	awayMenu.modifyMenu(ID_STATUS_AWAY_NOREPLY, bitmap);
	awayMenu.modifyMenu(ID_STATUS_AWAY_CUSTOM, IDB_SYS_OPTION);

	currentMenu = &statusMenu;
	statusMenu.TrackPopupMenu(nFlags, pt.x, pt.y, this);
	currentMenu = NULL;
}

void CIcqDlg::updateStatusIcon()
{
	KillTimer(IDT_STATUS);

	switch (myInfo.status) {
	case STATUS_ONLINE:
		m_btnStatus.setText(IDS_STATUS_ONLINE);
		break;
	case STATUS_OFFLINE:
		m_btnStatus.setText(IDS_STATUS_OFFLINE);
		break;
	case STATUS_AWAY:
		m_btnStatus.setText(IDS_STATUS_AWAY);
		break;
	case STATUS_INVIS:
		m_btnStatus.setText(IDS_STATUS_INVIS);
		break;
	}

	HICON hIcon = statusImgList.ExtractIcon(myInfo.status);
	m_btnStatus.SetIcon(hIcon);
	modifyNotifyIcon(hIcon);
}

void CIcqDlg::animateStatus()
{
	SetTimer(IDT_STATUS, 250, NULL);
	OnTimer(IDT_STATUS);
}

void CIcqDlg::hideWindow(BOOL hide)
{
	CRect rc;
	GetWindowRect(rc);
	int moves = 20;
	int xStep, yStep;
	int xEnd, yEnd;
	int cxScreen;

	switch (alignType) {
	case ALIGN_TOP:
		xStep = 0;
		xEnd = rc.left;
		if (hide) {
			yStep = -rc.bottom / moves;
			yEnd = -rc.Height() + 2;
		} else {
			yStep = -rc.top / moves;
			yEnd = 0;
		}
		break;
	case ALIGN_LEFT:
		yStep = 0;
		yEnd = rc.top;
		if (hide) {
			xStep = -rc.right / moves;
			xEnd = -rc.Width() + 2;
		} else {
			xStep = -rc.left / moves;
			xEnd = 0;
		}
		break;
	case ALIGN_RIGHT:
		yStep = 0;
		yEnd = rc.top;
		cxScreen = GetSystemMetrics(SM_CXSCREEN);
		if (hide) {
			xStep = (cxScreen - rc.left) / moves;
			xEnd = cxScreen - 2;
		} else {
			xStep = (cxScreen - rc.right) / moves;
			xEnd = cxScreen - rc.Width();
		}
		break;
	}

	for (int i = 0; i < moves; i++) {
		rc.left += xStep;
		rc.top += yStep;
		SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOSENDCHANGING);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
		Sleep(20);
	}
	SetWindowPos(NULL, xEnd, yEnd, 0, 0, SWP_NOSIZE);

	isHide = hide;
	if (!isHide)
		SetTimer(IDT_HOVER, 50, NULL);
}

void CIcqDlg::addPendingMsg(IcqMsg *msg)
{
	if (msgList.empty())
		SetTimer(IDT_BLINK, 400, NULL);
	if (!msg->isSysMsg() && !findPendingMsg(msg->uin))
		outbarCtrl.blinkImage(msg->uin);

	IcqLink::addPendingMsg(msg);
}

void CIcqDlg::onNewUINReply(uint32 uin)
{
	myInfo.uin = uin;
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win)
		((CRegWizard *) win)->onNewUINReply(uin);
}

void CIcqDlg::onContactListReply(UinList &l)
{
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win) {
		uinList = l;
		((CRegWizard *) win)->onContactListReply();
	}
}

void CIcqDlg::onLoginReply(uint16 error)
{
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win) {
		((CRegWizard *) win)->onLoginReply(error);
		return;
	}

	lastSeq = 0;
	if (error != LOGIN_SUCCESS) {
		KillTimer(IDT_RESEND);
		AfxMessageBox(IDS_WRONG_PASSWD);
		return;
	}

	SetTimer(IDT_KEEPALIVE, KEEPALIVE_TIMEOUT * 1000, NULL);

	myInfo.status = pendingStatus;
	updateStatusIcon();

	if (regMode == 0)
		serverSession()->modifyUser(&myInfo);
	else if (regMode == 1) {
		serverSession()->updateUser();
		while (!uinList.empty()) {
			uint32 uin = uinList.front();
			uinList.pop_front();
			serverSession()->updateContact(uin);
		}
	}
	regMode = -1;
}

void CIcqDlg::onUpdateContactReply(IcqContact *info)
{
	IcqLink::onUpdateContactReply(info);
	
	IcqContact *c = findContact(info->uin);
	if (c) {
		IcqDB::saveContact(*c);
		outbarCtrl.update(c->uin);
	}
	
	IcqWindow *win = findWindow(WIN_VIEW_DETAIL, info->uin, serverSession()->ackseq);
	if (win)
		((CViewDetailDlg *) win)->onUpdateContactReply(info);
}

void CIcqDlg::onUpdateUserReply()
{
	IcqDB::saveUser(myInfo);
}

void CIcqDlg::onUserOnline(uint32 uin, uint32 status, uint32 ip, uint16 port, uint32 realIP, bool multi)
{
	if (status != STATUS_ONLINE && status != STATUS_AWAY)
		return;

	IcqContact *c = findContact(uin, CONTACT_FRIEND);
	if (!c)
		c = addContact(uin);

	c->ip = ip;
	c->port = port;
	c->realIP = realIP;
	c->status = status;
	outbarCtrl.update(uin, TRUE);
	
	if (c->flags.test(CF_OPTION_INVISIBLE))
		OnStatusInvis();
	if (c->flags.test(CF_OPTION_GREETING) && !c->greeting.empty())
		sendMessage(MSG_TEXT, uin, c->greeting.c_str());
	
	if (!multi)
		onlineNotify(c);
}

void CIcqDlg::onUserOffline(uint32 uin)
{
	IcqLink::onUserOffline(uin);

	outbarCtrl.update(uin, TRUE);

	IcqContact *c = findContact(uin);
	if (c && c->flags.test(CF_OPTION_POPUP_OFFLINE)) {
		CString str;
		str.Format(IDS_NOTIFY_OFFLINE, c->nick.c_str(), c->uin);
		new CNotifyWnd(getApp()->largeImageList.ExtractIcon(c->face), str);
	}
}

void CIcqDlg::onStatusChanged(uint32 uin, uint32 status)
{
	if (status != STATUS_ONLINE && status != STATUS_AWAY)
		return;
	
	IcqContact *c = findContact(uin);
	if (c) {
		if (c->status != STATUS_ONLINE && status == STATUS_ONLINE)
			onlineNotify(c);
		c->status = status;
		outbarCtrl.update(uin);
	}
}

void CIcqDlg::onRecvMessage(IcqMsg *msg)
{
	uint8 type = msg->type;
	uint32 from = msg->uin;
	BOOL relay = (msg->flags & MF_RELAY);

	if (type == MSG_AUTH_ACCEPTED) {
		serverSession()->updateContact(from);
		addContact(from);
	}

	IcqContact *c = findContact(from);

	if (!c && !msg->isSysMsg()) {
		int n = outbarCtrl.getFolderCount();
		c = addContact(from, n - 2);
	}

	if (msg->isSysMsg()) {
		options.playSound(SOUND_SYSMSG);
		addPendingMsg(msg);
		IcqDB::saveMsg(*msg);
		return;
	}
	if (!c)
		return;

	if (c->status == STATUS_OFFLINE && !relay) {
		c->status = STATUS_ONLINE;
		outbarCtrl.update(c->uin, TRUE);
	}
	
	options.playSound(SOUND_MSG, c);
	if (type == MSG_TCP_REQUEST) {
		addPendingMsg(msg);
		return;
	}
	IcqDB::saveMsg(*msg);

	// auto reply
	if ((myInfo.status == STATUS_AWAY) &&
		options.flags.test(UF_AUTO_REPLY) &&
		!options.autoReplyText.empty() &&
		type != MSG_AUTO_REPLY)
		sendMessage(MSG_AUTO_REPLY, from, options.autoReplyText.c_str());

	IcqWindow *win;
	if (c->flags.test(CF_DIALOGMODE)) {
		win = findWindow(WIN_SEND_MESSAGE, from);
		if (!win) {
			CSendMsgDlg *pDlg = new CSendMsgDlg(c);
			pDlg->Create(IDD_SEND_MESSAGE);
			win = pDlg;
		}
		((CSendMsgDlg *) win)->onRecvMessage(*msg);
		delete msg;

	} else {
		addPendingMsg(msg);

		win = findWindow(WIN_VIEW_MESSAGE, from);
		if (win)
			((CViewMsgDlg *) win)->onRecvMessage();
		else if (options.flags.test(UF_AUTO_POPUP_MSG)) {
			CViewMsgDlg *dlg = new CViewMsgDlg(c);
			dlg->Create(IDD_VIEW_MESSAGE);
			win = dlg;
		}
	}
}

void CIcqDlg::onUserFound(PtrList *l)
{
	IcqWindow *win = findWindow(WIN_SEARCH_WIZARD, 0, serverSession()->ackseq);
	if (win)
		((CSearchWizard *) win)->onUserFound(l);
}

void CIcqDlg::onAddFriendReply(uint32 uin, uint8 result)
{
	uint16 seq = serverSession()->ackseq;
	IcqWindow *win;
	if (win = findWindow(WIN_SEARCH_WIZARD, uin, seq))
		((CSearchWizard *) win)->onAddFriendReply(result);
	else if ((win = findWindow(WIN_SYS_MESSAGE, 0, seq)) ||
			(win = findWindow(WIN_ADD_FRIEND, uin, seq)))
		((CSysMsgDlg *) win)->onAddFriendReply(result);
	else
		return;

	if (result == ADD_FRIEND_ACCEPTED) {
		serverSession()->updateContact(uin);
		addContact(uin);
	}
}

void CIcqDlg::setDefaultOptions()
{
	static const char *files[NR_SOUNDS] = {
		"global.wav",
		"msg.wav",
		"system.wav",
		"folder.wav",
	};
	for (int i = 0; i < NR_SOUNDS; i++)
		options.soundFiles[i] = getApp()->rootDir + "sound\\" + files[i];

	bitset<NR_USER_FLAGS> &flags = options.flags;
	flags.set(UF_ALWAYS_ON_TOP);
	flags.set(UF_TASKBAR_STYLE);
	flags.set(UF_ONLINE_NOTIFY);
	flags.set(UF_MONITOR_NETWORK);
	flags.set(UF_TASKBAR_ICON);
	flags.set(UF_USE_HOTKEY);
	flags.set(UF_DEFAULT_HOTKEY);
	flags.set(UF_SOUND_ON);

	CString str;
	int id;
	StrList l;
	for (id = IDS_QUICKREPLY_1; id <= IDS_QUICKREPLY_4; id++) {
		str.LoadString(id);
		l.push_back((LPCTSTR) str);
	}
	IcqDB::saveQuickReply(l);

	l.clear();
	for (id = IDS_AUTOREPLY_1; id <= IDS_AUTOREPLY_3; id++) {
		str.LoadString(id);
		l.push_back((LPCTSTR) str);
	}
	IcqDB::saveAutoReply(l);

	options.autoStatus = STATUS_AWAY;
	options.autoStatusTime = 10;

	outbarCtrl.removeAllFolders();
	str.LoadString(IDS_GROUP_FRIEND);
	outbarCtrl.addFolder(str);
	str.LoadString(IDS_GROUP_STRANGER);
	outbarCtrl.addFolder(str);
	str.LoadString(IDS_GROUP_BLACKLIST);
	outbarCtrl.addFolder(str);
	outbarCtrl.setSelFolder(0);
}

void CIcqDlg::onError()
{
	updateStatusIcon();
	logout();
}

void CIcqDlg::resolveHost()
{
	const char *host = options.host.c_str();

	if (options.flags.test(UF_USE_PROXY) && !options.proxy[options.proxyType].resolve)
		serverSession()->connect(host, options.port);
	else {
		struct in_addr addr;
		if ((addr.s_addr = inet_addr(host)) != INADDR_NONE)
			onHostFound(addr);
		else
			getHostByName(host);
	}
}

void CIcqDlg::initGUI()
{
	CString str;
	str.Format("%lu", myInfo.uin);
	SetWindowText(str);

#ifndef _DEBUG
	SetWindowPos(options.flags.test(UF_ALWAYS_ON_TOP) ? &wndTopMost : &wndNoTopMost,
		0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
#endif

	removeNotifyIcon();
	if (options.flags.test(UF_TASKBAR_ICON))
		addNotifyIcon();

	UnregisterHotKey(m_hWnd, ID_HOTKEY);
	if (options.flags.test(UF_USE_HOTKEY)) {
		uint32 hotKey = (options.flags.test(UF_DEFAULT_HOTKEY) ?
			HOTKEY_DEFAULT : options.hotKey);
		if (!RegisterHotKey(m_hWnd, ID_HOTKEY, HIWORD(hotKey), LOWORD(hotKey)))
			AfxMessageBox(IDS_FAILED_HOTKEY);
	}
	outbarCtrl.setShowOnlineOnly(options.flags.test(UF_SHOW_ONLINE));

	PtrList &l = PluginFactory::getPluginList();
	PtrList::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		IcqPlugin *p = (IcqPlugin *) *it;
		if (p->type == ICQ_PLUGIN_TOY)
			((ToyPlugin *) p)->init(icqLink, getApp()->getProfile(p->name.c_str()));
	}

	if (options.skin.empty())
		outbarCtrl.setSkin(NULL);
	else if (skin.load(getApp()->rootDir + "skins\\" + options.skin.c_str()))
		outbarCtrl.setSkin(&skin);
}

BOOL CIcqDlg::doRegWizard()
{
	destroyUser();

	SetTimer(IDT_RESEND, SEND_TIMEOUT * 1000, NULL);

	{
		CRegWizard wiz;
		wiz.DoModal();
		if (!wiz.isFinished)
			return FALSE;

		wiz.getData(&myInfo, &options);
		regMode = wiz.modeDlg.m_mode;
	}

	serverSession()->uin = myInfo.uin;
	IcqConfig::addUser(myInfo.uin);
	getApp()->setDBDir(myInfo.uin);

	setDefaultOptions();
	IcqDB::saveOptions(options);
	IcqDB::saveUser(myInfo);
	saveGroupInfo();

	if (regMode == 1) {
		UinList::iterator it, next;
		for (it = uinList.begin(); it != uinList.end(); it = next) {
			next = it;
			++next;
			uint32 uin = *it;
			IcqContact *c = findContact(uin);
			if (c)
				uinList.erase(it);
			else
				addContact(uin);
		}
	}
	saveGroupInfo();
	outbarCtrl.Invalidate();

	initGUI();
	login();
	return TRUE;
}

BOOL CIcqDlg::doLogin()
{
	CLoginDlg loginDlg;
	int result = loginDlg.DoModal();
	if (result == IDCANCEL)
		return FALSE;

	if (result == IDC_REGWIZARD)
		return doRegWizard();

	destroyUser();

	myInfo.uin = loginDlg.m_uin;
	serverSession()->uin = myInfo.uin;
	getApp()->setDBDir(myInfo.uin);

	if (!IcqDB::loadOptions(options) ||
		!IcqDB::loadUser(myInfo) ||
		!loadGroupInfo()) {
		if (AfxMessageBox(IDS_PROMPT_DB_CORRUPTED, MB_YESNO) == IDYES)
			return doRegWizard();
		return FALSE;
	}

	SetTimer(IDT_RESEND, SEND_TIMEOUT * 1000, NULL);

	CString passwd = loginDlg.m_passwd;
	while (passwd != myInfo.passwd.c_str()) {
		CLoginAgainDlg againDlg(loginDlg.m_uin);
		if (againDlg.DoModal() != IDOK)
			return FALSE;

		passwd = againDlg.m_passwd;
	}

	initGUI();
	login(options.flags.test(UF_LOGIN_INVIS) ? STATUS_INVIS : STATUS_ONLINE);
	return TRUE;
}

void CIcqDlg::destroyUser()
{
	IcqLink::destroyUser();

	UnregisterHotKey(m_hWnd, ID_HOTKEY);
	regMode = -1;
}

IcqMsg *CIcqDlg::fetchMsg(uint32 uin)
{
	IcqMsg *msg = findPendingMsg(uin);
	if (msg)
		msgList.remove(msg);

	if (!findPendingMsg(uin)) {
		if (uin)
			outbarCtrl.blinkImage(uin, FALSE);
		else
			m_btnSysMsg.SetIcon(iconSysMsg);
	}
	if (msgList.empty()) {
		KillTimer(IDT_BLINK);
		updateStatusIcon();
	}
	return msg;
}

void CIcqDlg::showOptions(UINT iSelectPage)
{
	CSysOptionDlg win(this, iSelectPage);
	if (win.DoModal() == IDOK) {
		IcqDB::saveOptions(options);
		initGUI();
	}
}

void CIcqDlg::onStatusAway()
{
	if (myInfo.status == STATUS_OFFLINE)
		login(STATUS_AWAY);
	else {
		if (myInfo.status != STATUS_AWAY) {
			myInfo.status = STATUS_AWAY;
			serverSession()->changeStatus(myInfo.status);
		}
		updateStatusIcon();
	}
}

void CIcqDlg::onNotifyDblClk()
{
	if (msgList.empty()) {
		if (isHide) {
			hideWindow(FALSE);
			SetTimer(IDT_HOVER, 5000, NULL);
		} else {
			ShowWindow(SW_NORMAL);
			SetForegroundWindow();
		}
	} else {
		IcqMsg *msg = (IcqMsg *) msgList.front();

		if (msg->isSysMsg())
			OnSysMessage();
		else {
			IcqContact *c = findContact(msg->uin);
			if (c)
				onItemDoubleClicked(c);
		}
	}
}

void CIcqDlg::saveGroupInfo()
{
	IcqDB::saveGroupInfo(outbarCtrl);
}

BOOL CIcqDlg::loadGroupInfo()
{
	if (!IcqDB::loadContact(contactList))
		return FALSE;
	
	if (!IcqDB::loadGroupInfo(outbarCtrl) || outbarCtrl.getFolderCount() < 3)
		return FALSE;
	return TRUE;
}

BOOL CIcqDlg::loadPlugins()
{
	CString dir = getApp()->rootDir + "plugins\\*.dll";
	CFileFind finder;
	BOOL cont = finder.FindFile(dir);

	while (cont) {
		cont = finder.FindNextFile();
		HINSTANCE module = AfxLoadLibrary(finder.GetFilePath());
		if (!module)
			continue;

		GETTYPE getType = (GETTYPE) GetProcAddress(module, "getType");
		if (getType) {
			int type = getType();
			IcqPlugin *plugin = NULL;
			if (type == ICQ_PLUGIN_NET) {
				NetPlugin *p = new NetPlugin;
				p->getNameIcon = (GETNAMEICON) GetProcAddress(module, "getNameIcon");
				p->createSession = (CREATESESSION) GetProcAddress(module, "createSession");
				plugin = p;
			} else if (type == ICQ_PLUGIN_TOY) {
				ToyPlugin *p = new ToyPlugin;
				p->getNameIcon = (GETNAMEICON) GetProcAddress(module, "getNameIcon");
				p->init = (INIT) GetProcAddress(module, "init");
				p->configure = (CONFIGURE) GetProcAddress(module, "configure");
				p->about = (ABOUT) GetProcAddress(module, "about");
				plugin = p;
			}

			plugin->type = type;
			plugin->name = finder.GetFileTitle();
			PluginFactory::registerPlugin(plugin);
		}
	}

	HICON icon = AfxGetApp()->LoadIcon(IDI_PLUGIN);

	dir = getApp()->rootDir + "plugins\\*.ini";
	cont = finder.FindFile(dir);
	while (cont) {
		cont = finder.FindNextFile();

		ExePlugin *p = new ExePlugin;
		p->type = ICQ_PLUGIN_EXE;
		p->name = finder.GetFileTitle();
		p->icon = icon;
		const char *name = p->name.c_str();
		CString file = finder.GetFilePath();

		char buf[1024];
		GetPrivateProfileString(name, "name", name, buf, sizeof(buf), file);
		p->locName = buf;
		GetPrivateProfileString(name, "client", NULL, buf, sizeof(buf), file);
		p->clientCmd = buf;
		GetPrivateProfileString(name, "server", NULL, buf, sizeof(buf), file);
		p->serverCmd = buf;

		PluginFactory::registerPlugin(p);
	}

	return TRUE;
}

void CIcqDlg::renamed(int obj, int index, const char *text) 
{
	if (obj == OutBarCtrl::HT_FOLDER)
		saveGroupInfo();
	else if (obj == OutBarCtrl::HT_ITEM) {
		IcqContact *c = outbarCtrl.contact(index);
		if (c) {
			c->nick = text;
			IcqDB::saveContact(*c);
		}
	}
}

void CIcqDlg::itemDragged(int item, int toFolder)
{
	IcqContact *c = outbarCtrl.contact(item);
	int fromFolder = outbarCtrl.getSelFolder();
	int n = outbarCtrl.getFolderCount();

	if (fromFolder >= n - 2 && toFolder < n - 2)
		addFriend(c, toFolder);
	else if (fromFolder < n - 2 && toFolder >= n - 2)
		delFriend(c, toFolder);
	else {
		c->type = (toFolder == n - 1 ? CONTACT_IGNORE : CONTACT_STRANGER);
		outbarCtrl.removeContact(c->uin);
		outbarCtrl.addContact(toFolder, c);
		saveGroupInfo();
	}
}

void CIcqDlg::selFolderChanged(int folder)
{
	options.playSound(SOUND_FOLDER);
}

void CIcqDlg::itemClicked(int item)
{
	curContact = outbarCtrl.contact(item);

	CGfxPopupMenu menu;
	CMenu tmp;

	int sel = outbarCtrl.getSelFolder();
	int n = outbarCtrl.getFolderCount();
	if (sel < n - 2)
		tmp.LoadMenu(IDR_CONTACT);
	else if (sel < n - 1)
		tmp.LoadMenu(IDR_STRANGER);
	else
		tmp.LoadMenu(IDR_IGNORE);
	menu.Attach(*tmp.GetSubMenu(0));

	if (sel < n - 2) {
		UINT id = ID_PLUGIN;
		PtrList &l = PluginFactory::getPluginList();
		PtrList::iterator it;
		for (it = l.begin(); it != l.end(); ++it) {
			IcqPlugin *plugin = (IcqPlugin *) *it;
			string name;
			HICON icon = NULL;
			int type = plugin->type;
			if (type == ICQ_PLUGIN_NET) {
				NetPlugin *p = (NetPlugin *) plugin;
				if (p->getNameIcon)
					icon = (HICON) p->getNameIcon(name);
				else
					name = p->name;
			} else if (type == ICQ_PLUGIN_EXE) {
				ExePlugin *p = (ExePlugin *) plugin;
				name = p->locName;
				icon = (HICON) p->icon;
			} else
				continue;

			menu.AppendMenu(0, id, name.c_str());
			menu.modifyMenu(id++, icon);
		}
	}

	menu.modifyMenu(ID_SEND_MSG, IDB_SEND_MSG);
	menu.modifyMenu(ID_SEND_EMAIL, IDB_SEND_EMAIL);
	menu.modifyMenu(ID_HOMEPAGE, IDB_HOMEPAGE);
	menu.modifyMenu(ID_MSG_HISTORY, IDB_MSG_HISTORY);
	menu.modifyMenu(ID_VIEW_DETAIL, IDB_VIEW_DETAIL);
	menu.modifyMenu(ID_ADD_AS_FRIEND, IDB_ADD);

	menu.EnableMenuItems(&menu, this);

	CPoint pt;
	::GetCursorPos(&pt);
	currentMenu = &menu;
	menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
	currentMenu = NULL;
}

void CIcqDlg::rightButtonDown(int obj, int index)
{
	CGfxPopupMenu menu;
	CMenu tmp;
	tmp.LoadMenu(IDR_OUTBAR);
	menu.Attach(*tmp.GetSubMenu(0));

	menu.modifyMenu(ID_GFX_SMALLICON, IDB_SMALLICON);
	menu.modifyMenu(ID_GFX_LARGEICON, IDB_LARGEICON);
	menu.modifyMenu(ID_GFX_ADDGROUP, IDB_ADDGROUP);
	menu.modifyMenu(ID_GFX_FONTCOLOR, IDB_FONTCOLOR);
	menu.modifyMenu(ID_GFX_BACKGROUND, IDB_BACKGROUND);
	menu.appendMenu(ID_GFX_SHOWONLINE, IDB_SHOWONLINE, outbarCtrl.showOnlineOnly() ? IDS_SHOWALL : IDS_SHOWONLINE);

	if (obj == OutBarCtrl::HT_ITEM) {
		menu.AppendMenu(MF_SEPARATOR);
		menu.appendMenu(ID_GFX_REMOVE, IDB_REMOVEITEM, IDS_REMOVEITEM);
		menu.appendMenu(ID_GFX_RENAME, IDB_RENAME, IDS_RENAMEITEM);

	} else if (obj == OutBarCtrl::HT_FOLDER) {
		int n = outbarCtrl.getFolderCount();
		if (index > 0 && index < n - 2) {
			menu.AppendMenu(MF_SEPARATOR);
			menu.appendMenu(ID_GFX_REMOVE, IDB_REMOVEGROUP, IDS_REMOVEGROUP);
			menu.appendMenu(ID_GFX_RENAME, IDB_RENAME, IDS_RENAMEGROUP);
		}
	}

	menu.EnableMenuItems(&menu, this);
	CPoint pt;
	::GetCursorPos(&pt);
	currentMenu = &menu;
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	currentMenu = NULL;
}

void CIcqDlg::itemDoubleClicked(int item)
{
	onItemDoubleClicked(outbarCtrl.contact(item));
}

void CIcqDlg::onItemDoubleClicked(IcqContact *c)
{
	int sel = outbarCtrl.getSelFolder();
	int n = outbarCtrl.getFolderCount();
	
	IcqMsg *msg = fetchMsg(c->uin);
	if (msg && msg->type == MSG_TCP_REQUEST) {
		if (sel < n - 2) {
			CSendRequestDlg *dlg = new CSendRequestDlg(msg);
			dlg->Create(IDD_SEND_REQUEST);
			delete msg;
		}
	} else {
		curContact = c;
		
		if (sel < n - 1) {
			if (msg)
				msgList.push_front(msg);
			OnSendMessage();
		} else
			OnViewDetail();
	}
}

HANDLE CIcqDlg::getHostByName(const char *name)
{
	return WSAAsyncGetHostByName(m_hWnd, WM_HOSTFOUND, name, hostBuf, sizeof(hostBuf));
}

void CIcqDlg::DoDataExchange(CDataExchange* pDX)
{
	CCaptionBtnDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcqDlg)
	DDX_Control(pDX, ID_STATISTICS, m_btnStat);
	DDX_Control(pDX, ID_ABOUT, m_btnAbout);
	DDX_Control(pDX, IDC_MAIN_MENU, m_btnMain);
	DDX_Control(pDX, IDC_STATUS, m_btnStatus);
	DDX_Control(pDX, IDC_SYS_MESSAGE, m_btnSysMsg);
	DDX_Control(pDX, IDC_FIND_CONTACT, m_btnSearch);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIcqDlg, CCaptionBtnDlg)
	//{{AFX_MSG_MAP(CIcqDlg)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_SEND_MSG, OnSendMessage)
	ON_COMMAND(ID_VIEW_DETAIL, OnViewDetail)
	ON_BN_CLICKED(IDC_FIND_CONTACT, OnFindContact)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SYS_MESSAGE, OnSysMessage)
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_MAIN_MENU, OnMainMenu)
	ON_COMMAND(ID_GFX_SMALLICON, OnGfxSmallIcon)
	ON_UPDATE_COMMAND_UI(ID_GFX_SMALLICON, OnUpdateGfxSmallIcon)
	ON_COMMAND(ID_GFX_LARGEICON, OnGfxLargeIcon)
	ON_UPDATE_COMMAND_UI(ID_GFX_LARGEICON, OnUpdateGfxLargeIcon)
	ON_COMMAND(ID_GFX_FONTCOLOR, OnGfxFontColor)
	ON_COMMAND(ID_GFX_BACKGROUND, OnGfxBackground)
	ON_COMMAND(ID_GFX_ADDGROUP, OnGfxAddGroup)
	ON_UPDATE_COMMAND_UI(ID_GFX_ADDGROUP, OnUpdateGfxAddGroup)
	ON_COMMAND(ID_GFX_REMOVE, OnGfxRemove)
	ON_COMMAND(ID_GFX_RENAME, OnGfxRename)
	ON_COMMAND(ID_MODIFY_INFO, OnModifyInfo)
	ON_COMMAND(ID_GFX_SHOWONLINE, OnGfxShowOnline)
	ON_BN_CLICKED(IDC_STATUS, OnStatus)
	ON_COMMAND(ID_STATUS_ONLINE, OnStatusOnline)
	ON_COMMAND(ID_STATUS_OFFLINE, OnStatusOffline)
	ON_COMMAND(ID_STATUS_INVIS, OnStatusInvis)
	ON_COMMAND(ID_SYS_OPTION, OnSysOption)
	ON_COMMAND(ID_STATUS_AWAY_CUSTOM, OnStatusAwayCustom)
	ON_COMMAND(ID_STATUS_AWAY_NOREPLY, OnStatusAwayNoReply)
	ON_WM_MEASUREITEM()
	ON_COMMAND(ID_REG_WIZARD, OnRegWizard)
	ON_COMMAND(ID_CHANGE_USER, OnChangeUser)
	ON_BN_CLICKED(ID_ABOUT, OnAbout)
	ON_WM_DESTROY()
	ON_WM_MOVING()
	ON_COMMAND(ID_SEND_EMAIL, OnSendEmail)
	ON_COMMAND(ID_HOMEPAGE, OnHomePage)
	ON_COMMAND(ID_ADD_AS_FRIEND, OnAddAsFriend)
	ON_COMMAND(ID_MSG_HISTORY, OnMsgHistory)
	ON_COMMAND(ID_MESSAGE_MGR, OnMessageMgr)
	ON_COMMAND(ID_BROADCAST_MSG, OnBroadcastMsg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOSTFOUND, OnHostFound)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(WM_NOTIFYICON, OnNotifyIcon)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_COMMAND_RANGE(ID_STATUS_AWAY, ID_STATUS_AWAY + 9, OnStatusAway)
	ON_COMMAND_RANGE(ID_PLUGIN, ID_PLUGIN + 1000, OnPlugin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcqDlg message handlers

BOOL CIcqDlg::OnInitDialog()
{
	CCaptionBtnDlg::OnInitDialog();

	toolTip.Create(this);
	CRect rc;
	GetWindowRect(rc);
	ScreenToClient(rc);
	toolTip.AddTool(this, LPSTR_TEXTCALLBACK, rc, ID_TOOLTIP);

	m_btnSearch.SetIcon(IDI_FIND);
	m_btnSysMsg.SetIcon(iconSysMsg);
	m_btnAbout.SetIcon(IDI_ABOUT);
	m_btnStat.SetIcon(IDI_STAT);
	m_btnMain.SetFlat(FALSE, FALSE);

	statusImgList.Create(16, 16, ILC_MASK | ILC_COLOR8, 4, 0);
	UINT id[] = {
		IDB_STATUS_ONLINE,
		IDB_STATUS_OFFLINE,
		IDB_STATUS_AWAY,
		IDB_STATUS_INVIS
	};
	CBitmap bm;
	for (int i = 0; i < 4; i++) {
		bm.LoadBitmap(id[i]);
		statusImgList.Add(&bm, RGB(192, 192, 192));
		bm.DeleteObject();
	}

	m_btnStatus.SetTooltipText(IDS_CHANGE_STATUS);
	m_btnMain.SetTooltipText(IDS_MAINMENU);
	m_btnSearch.SetTooltipText(IDS_SEARCH);
	m_btnSysMsg.SetTooltipText(IDS_SYSMSG);
	m_btnAbout.SetTooltipText(IDS_ABOUT);
	m_btnStat.SetTooltipText(IDS_STATISTICS);

	outbarCtrl.CreateEx(WS_EX_CLIENTEDGE, NULL, NULL, WS_VISIBLE | WS_CHILD,
		CRect(0, 0, 0, 0), this, IDC_OUTBAR);

	loadPlugins();

	if (!doLogin()) {
		EndDialog(IDCANCEL);
		return TRUE;
	}

	outbarCtrl.SetFocus();
	return FALSE;
}

void CIcqDlg::OnSize(UINT nType, int cx, int cy) 
{
	CCaptionBtnDlg::OnSize(nType, cx, cy);

	int y = cy;
	CRect rc;

	m_btnMain.GetWindowRect(rc);
	ScreenToClient(rc);
	y -= rc.Height();
	m_btnMain.MoveWindow(rc.left, y, rc.Width(), rc.Height(), FALSE);
	
	m_btnStatus.GetWindowRect(rc);
	ScreenToClient(rc);
	m_btnStatus.MoveWindow(rc.left, y, cx - rc.left, rc.Height(), FALSE);

	m_btnStat.GetWindowRect(rc);
	ScreenToClient(rc);
	y -= rc.Height() + 2;
	m_btnStat.MoveWindow(rc.left, y, rc.Width(), rc.Height(), FALSE);

	m_btnSysMsg.GetWindowRect(rc);
	ScreenToClient(rc);
	m_btnSysMsg.MoveWindow(rc.left, y, cx - rc.left, rc.Height(), FALSE);

	m_btnAbout.GetWindowRect(rc);
	ScreenToClient(rc);
	y -= rc.Height() + 2;
	m_btnAbout.MoveWindow(rc.left, y, rc.Width(), rc.Height(), FALSE);
	
	m_btnSearch.GetWindowRect(rc);
	ScreenToClient(rc);
	m_btnSearch.MoveWindow(rc.left, y, cx - rc.left, rc.Height(), FALSE);

	y -= 2;
	outbarCtrl.MoveWindow(0, 0, cx, y, FALSE);

	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}

void CIcqDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = 95;
	lpMMI->ptMinTrackSize.y = 245;
	lpMMI->ptMaxTrackSize.x = 340;
	
	CCaptionBtnDlg::OnGetMinMaxInfo(lpMMI);
}

UINT CIcqDlg::OnNcHitTest(CPoint point) 
{
	UINT res = CCaptionBtnDlg::OnNcHitTest(point);
	if (res == HTCLIENT)
		return HTCAPTION;

	switch (alignType) {
	case ALIGN_TOP:
		if (res == HTLEFT || res == HTRIGHT || res == HTTOP ||
			res == HTTOPLEFT || res == HTTOPRIGHT)
			res = HTERROR;
		else if (res == HTBOTTOMLEFT || res == HTBOTTOMRIGHT)
			res = HTBOTTOM;
		if (isHide && res == HTBOTTOM)
			hideWindow(FALSE);
		break;
	case ALIGN_LEFT:
		if (res == HTTOP || res == HTBOTTOM || res == HTLEFT ||
			res == HTTOPLEFT || res == HTBOTTOMLEFT)
			res = HTERROR;
		else if (res == HTTOPRIGHT || res == HTBOTTOMRIGHT)
			res = HTRIGHT;
		if (isHide && res == HTRIGHT)
			hideWindow(FALSE);
		break;
	case ALIGN_RIGHT:
		if (res == HTTOP || res == HTBOTTOM || res == HTRIGHT ||
			res == HTTOPRIGHT || res == HTBOTTOMRIGHT)
			res = HTERROR;
		else if (res == HTTOPLEFT || res == HTBOTTOMLEFT)
			res = HTLEFT;
		if (isHide && res == HTLEFT)
			hideWindow(FALSE);
		break;
	}
	return res;
}

BOOL CIcqDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ((myInfo.status != STATUS_OFFLINE) &&
		(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST) ||
		(pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) &&
		options.flags.test(UF_AUTO_SWITCH_STATUS)) {

		if (myInfo.status == STATUS_AWAY && options.flags.test(UF_AUTO_CANCEL_AWAY))
			OnStatusOnline();
		if (options.autoStatusTime > 0)
			SetTimer(IDT_AUTO_SWITCH_STATUS, options.autoStatusTime * 60000, NULL);
	}

	if (pMsg->hwnd == m_hWnd)
		toolTip.RelayEvent(pMsg);

	return CCaptionBtnDlg::PreTranslateMessage(pMsg);
}

void CIcqDlg::OnSendMessage() 
{
	IcqContact *c = curContact;
	IcqMsg *msg = fetchMsg(c->uin);

	IcqWindow *win;
	if (c->flags.test(CF_DIALOGMODE) || !msg) {
		outbarCtrl.blinkImage(c->uin, FALSE);

		CSendMsgDlg *pDlg;
		win = findWindow(WIN_SEND_MESSAGE, c->uin);
		if (!win) {
			pDlg = new CSendMsgDlg(c);
			pDlg->Create(IDD_SEND_MESSAGE);
		} else {
			pDlg = (CSendMsgDlg *) win;
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->BringWindowToTop();
		}
		if (c->flags.test(CF_DIALOGMODE)) {
			while (msg) {
				pDlg->onRecvMessage(*msg);
				delete msg;
				msg = fetchMsg(c->uin);
			}
		}
	} else {
		// fake that we have not fetched it.
		msgList.push_front(msg);

		CViewMsgDlg *pDlg;
		win = findWindow(WIN_VIEW_MESSAGE, c->uin);
		if (!win) {
			pDlg = new CViewMsgDlg(c);
			pDlg->Create(IDD_VIEW_MESSAGE);
		} else {
			pDlg = (CViewMsgDlg *) win;
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->BringWindowToTop();
		}
	}
}

void CIcqDlg::OnViewDetail() 
{
	CViewDetailDlg *win = new CViewDetailDlg(curContact->uin);
	win->Create();
}

void CIcqDlg::OnFindContact() 
{
	IcqWindow *win = findWindow(WIN_SEARCH_WIZARD);
	if (!win) {
		CSearchWizard *dlg = new CSearchWizard;
		dlg->Create();
	}
}

void CIcqDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_RESEND)
		checkSendQueue();
	else if (nIDEvent == IDT_KEEPALIVE)
		serverSession()->sendKeepAlive();
	else if (nIDEvent == IDT_STATUS) {
		static int frame = 0;
		HICON hIcon = statusImgList.ExtractIcon(frame);
		m_btnStatus.SetIcon(hIcon);
		modifyNotifyIcon(hIcon);
		if (++frame >= 3)
			frame = 0;

	} else if (nIDEvent == IDT_BLINK) {
		static int frame = 0;
		frame ^= 1;

		IcqMsg *msg = (IcqMsg *) msgList.front();
		HICON icon = NULL;

		if (msg->isSysMsg()) {
			icon = (frame ? iconSysMsg : iconBlank);
			m_btnSysMsg.SetIcon(icon);
		} else {
			IcqContact *c = findContact(msg->uin);
			if (c)
				icon = (frame ? getApp()->smallImageList.ExtractIcon(c->face) : iconBlank);
		}
		modifyNotifyIcon(icon);

	} else if (nIDEvent == IDT_DBLCLK) {
		KillTimer(nIDEvent);

		CPoint pt;
		GetCursorPos(&pt);
		SetForegroundWindow();
		popupStatusMenu(TPM_BOTTOMALIGN | TPM_RIGHTALIGN, pt);

	} else if (nIDEvent == IDT_HOVER) {
		CPoint pt;
		GetCursorPos(&pt);
		CRect rc;
		GetWindowRect(rc);
		rc.InflateRect(10, 10);
		if (!rc.PtInRect(pt)) {
			KillTimer(nIDEvent);
			hideWindow();
		}
	} else if (nIDEvent == IDT_AUTO_SWITCH_STATUS) {
		KillTimer(nIDEvent);
		if (options.flags.test(UF_AUTO_SWITCH_STATUS)) {
			switch (options.autoStatus) {
			case STATUS_AWAY:
				onStatusAway();
				break;
			case STATUS_INVIS:
				OnStatusInvis();
				break;
			case STATUS_OFFLINE:
				OnStatusOffline();
				break;
			}
		}

	} else
		CCaptionBtnDlg::OnTimer(nIDEvent);
}

void CIcqDlg::OnSysMessage() 
{
	IcqMsg *msg = fetchMsg(0);
	IcqWindow *win;	
	
	if (!msg) {
		CSysHistoryDlg *pDlg;
		win = findWindow(WIN_SYS_HISTORY);
		if (!win) {
			pDlg = new CSysHistoryDlg;
			pDlg->Create(IDD_SYS_HISTORY);
		} else {
			pDlg = (CSysHistoryDlg *) win;
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->BringWindowToTop();
		}
	} else {
		CSysMsgDlg *win = new CSysMsgDlg(msg);
		win->Create(IDD_SYS_MESSAGE);
	}
}

void CIcqDlg::OnMainMenu() 
{
	CGfxPopupMenu menu;
	CMenu tmp;
	tmp.LoadMenu(IDR_MAIN);
	menu.Attach(*tmp.GetSubMenu(0));

	menu.modifyMenu(ID_CHANGE_USER, IDB_CHANGE_USER);
	menu.modifyMenu(ID_REG_WIZARD, IDB_REG_WIZARD);
	menu.modifyMenu(ID_MODIFY_INFO, IDB_VIEW_DETAIL);
	menu.modifyMenu(ID_SYS_OPTION, IDB_SYS_OPTION);
	menu.modifyMenu(ID_APP_EXIT, IDB_EXIT);

	CRect rc;
	m_btnMain.GetWindowRect(rc);
	popupMainMenu(TPM_BOTTOMALIGN, rc.TopLeft());
}

void CIcqDlg::OnGfxSmallIcon() 
{
	outbarCtrl.setIconView(FALSE);
	saveGroupInfo();
}

void CIcqDlg::OnUpdateGfxSmallIcon(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(outbarCtrl.isLargeIconView());
}

void CIcqDlg::OnGfxLargeIcon() 
{
	outbarCtrl.setIconView(TRUE);
	saveGroupInfo();
}

void CIcqDlg::OnUpdateGfxLargeIcon(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!outbarCtrl.isLargeIconView());
}

void CIcqDlg::OnGfxFontColor() 
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK) {
		outbarCtrl.setFontColor(dlg.GetColor());
		saveGroupInfo();
	}
}

void CIcqDlg::OnGfxBackground() 
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK) {
		outbarCtrl.setBgColor(dlg.GetColor());
		saveGroupInfo();
	}
}

void CIcqDlg::OnGfxAddGroup() 
{
	outbarCtrl.addFolderUI();
	saveGroupInfo();
}

void CIcqDlg::OnGfxRename()
{
	outbarCtrl.renameUI();
}

void CIcqDlg::OnUpdateGfxAddGroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(outbarCtrl.getSelFolder() < outbarCtrl.getFolderCount() - 2);
}

void CIcqDlg::OnGfxRemove() 
{
	int obj = outbarCtrl.hitObj();
	int index = outbarCtrl.hitIndex();

	if (obj == OutBarCtrl::HT_FOLDER) {
		if (outbarCtrl.getItemCount(index) > 0)
			AfxMessageBox(IDS_GROUP_NOT_EMPTY);
		else {
			outbarCtrl.removeFolder(index);
			saveGroupInfo();
		}
	} else if (obj == OutBarCtrl::HT_ITEM) {
		IcqContact *c = outbarCtrl.contact(index);
		int n = outbarCtrl.getFolderCount();
		if (outbarCtrl.getSelFolder() < n - 2)
			delFriend(c);
		else {
			outbarCtrl.removeItem(index);
			contactList.remove(c);
			IcqDB::delContact(c->uin);
			saveGroupInfo();
			delete c;
		}
	}
}

void CIcqDlg::OnModifyInfo() 
{
	CModifyDetailDlg *win = new CModifyDetailDlg;
	win->Create();
}

void CIcqDlg::OnGfxShowOnline() 
{
	bool b = !outbarCtrl.showOnlineOnly();
	outbarCtrl.setShowOnlineOnly(b);
	options.flags.set(UF_SHOW_ONLINE, b);
	IcqDB::saveOptions(options);
}

void CIcqDlg::OnStatus() 
{
	CRect rc;
	m_btnStatus.GetWindowRect(rc);
	popupStatusMenu(TPM_BOTTOMALIGN, rc.TopLeft());
}

void CIcqDlg::OnStatusOnline() 
{
	if (myInfo.status == STATUS_OFFLINE)
		login();
	else {
		if (myInfo.status != STATUS_ONLINE) {
			myInfo.status = STATUS_ONLINE;
			serverSession()->changeStatus(myInfo.status);
		}
		updateStatusIcon();
	}
}

void CIcqDlg::OnStatusOffline() 
{
	if (myInfo.status != STATUS_OFFLINE)
		logout();
	updateStatusIcon();
}

void CIcqDlg::OnStatusInvis() 
{
	if (myInfo.status == STATUS_OFFLINE)
		login(STATUS_INVIS);
	else {
		if (myInfo.status != STATUS_INVIS) {
			myInfo.status = STATUS_INVIS;
			serverSession()->changeStatus(myInfo.status);
		}
		destroySession();
		updateStatusIcon();
	}
}

void CIcqDlg::OnStatusAwayNoReply() 
{
	options.flags.set(UF_AUTO_REPLY, false);
	IcqDB::saveOptions(options);
	onStatusAway();
}

void CIcqDlg::OnSysOption() 
{
	showOptions();
}

void CIcqDlg::OnStatusAwayCustom() 
{
	showOptions(1);
}

void CIcqDlg::OnStatusAway(UINT nID)
{
	int pos = nID - ID_STATUS_AWAY;

	StrList l;
	IcqDB::loadAutoReply(l);

	StrList::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		if (pos-- <= 0)
			break;
	}

	if (pos < 0) {
		options.flags.set(UF_AUTO_REPLY);
		options.autoReplyText = *it;
		IcqDB::saveOptions(options);

		onStatusAway();
	}
}

void CIcqDlg::OnPlugin(UINT nID)
{
	int pos = nID - ID_PLUGIN;
	PtrList &l = PluginFactory::getPluginList();
	PtrList::iterator it;
	IcqPlugin *p = NULL;
	int i = 0;
	for (it = l.begin(); it != l.end(); ++it) {
		p = (IcqPlugin *) *it;
		if (p->type == ICQ_PLUGIN_NET || p->type == ICQ_PLUGIN_EXE) {
			if (++i > pos)
				break;
		}
	}
	
	if (it != l.end()) {
		p = (IcqPlugin *) *it;
		CSendRequestDlg *dlg = new CSendRequestDlg(p->name.c_str(), curContact);
		dlg->Create(IDD_SEND_REQUEST);
	}
}

LRESULT CIcqDlg::OnHostFound(WPARAM wParam, LPARAM lParam)
{
	WORD error = WSAGETASYNCERROR(lParam);
	struct in_addr addr;
	addr.s_addr = INADDR_NONE;

	if (error == 0) {
		struct hostent *hent = (struct hostent *) hostBuf;
		addr = *(struct in_addr *) hent->h_addr;
	}

	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win)
		((CRegWizard *) win)->onHostFound(addr);
	else
		onHostFound(addr);

	return TRUE;
}

LRESULT CIcqDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ID_HOTKEY)
		onNotifyDblClk();

	return 1;
}

LRESULT CIcqDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	if (wParam != ID_NOTIFYICON || !myInfo.uin)
		return 0;

	if (lParam == WM_LBUTTONDOWN)
		SetTimer(IDT_DBLCLK, GetDoubleClickTime(), NULL);
	else if (lParam == WM_RBUTTONUP) {
		CPoint pt;
		GetCursorPos(&pt);
		SetForegroundWindow();
		popupMainMenu(TPM_BOTTOMALIGN | TPM_RIGHTALIGN, pt);

	} else if (lParam == WM_LBUTTONDBLCLK) {
		KillTimer(IDT_DBLCLK);
		onNotifyDblClk();
	}

	return 1;
}

BOOL CIcqDlg::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *) pNMHDR;

	if (pNMHDR->idFrom == ID_TOOLTIP) {
		if (serverSession()->sessionCount) {
			CString str;
			str.Format(IDS_NR_ONLINES, serverSession()->sessionCount);
			strcpy(pTTT->szText, str);
		} else {
			pTTT->lpszText = (LPSTR) (LPCTSTR) AfxGetAppName();
			pTTT->hinst = NULL;
		}
	}
	return TRUE;
}

void CIcqDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	if (lpMeasureItemStruct->CtlType == ODT_MENU && currentMenu)
		currentMenu->MeasureItem(lpMeasureItemStruct);
	else
		CCaptionBtnDlg::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CIcqDlg::OnRegWizard() 
{
	if (!doRegWizard())
		EndDialog(IDCANCEL);
}

void CIcqDlg::OnChangeUser() 
{
	doLogin();
	outbarCtrl.Invalidate();
}

void CIcqDlg::OnAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CIcqDlg::OnDestroy() 
{
	destroyUser();
	removeNotifyIcon();

	CCaptionBtnDlg::OnDestroy();
}

void CIcqDlg::OnMoving(UINT fwSide, LPRECT pRect) 
{
	if (!options.flags.test(UF_TASKBAR_STYLE)) {
		CCaptionBtnDlg::OnMoving(fwSide, pRect);
		return;
	}

	CRect rcOld, rcWorkArea;
	CPoint pt;
	GetWindowRect(rcOld);
	SystemParametersInfo(SPI_GETWORKAREA, 0, rcWorkArea, 0);
	GetCursorPos(&pt);
	int cxScreen = GetSystemMetrics(SM_CXSCREEN);

	if (alignType == ALIGN_NONE) {
		if (pt.y - rcWorkArea.top < 20) {
			alignType = ALIGN_TOP;
			pRect->top = 0;
		} else if (pt.x - rcWorkArea.left < 20) {
			alignType = ALIGN_LEFT;
			pRect->left = 0;
			winNormalSize = rcOld.Size();
		} else if (rcWorkArea.right - pt.x < 20) {
			alignType = ALIGN_RIGHT;
			pRect->right = cxScreen;
			winNormalSize = rcOld.Size();
		}
		if (alignType != ALIGN_NONE)
			SetTimer(IDT_HOVER, 50, NULL);
	}

	if (alignType == ALIGN_TOP) {
		if (pRect->top > 0)
			alignType = ALIGN_NONE;
		else {
			pRect->top = 0;
			pRect->bottom = rcOld.Height();
		}
	} else if (alignType == ALIGN_LEFT) {
		if (pRect->left > 0) {
			pRect->right = pRect->left + winNormalSize.cx;
			pRect->bottom = pRect->top + winNormalSize.cy;
			alignType = ALIGN_NONE;
		} else {
			pRect->left = 0;
			pRect->right = rcOld.Width();
			pRect->top = 0;
			pRect->bottom = rcWorkArea.bottom;
		}
	} else if (alignType == ALIGN_RIGHT) {
		if (pRect->right < cxScreen) {
			pRect->left = pRect->right - winNormalSize.cx;
			pRect->bottom = pRect->top + winNormalSize.cy;
			alignType = ALIGN_NONE;
		} else {
			pRect->right = cxScreen;
			pRect->left = pRect->right - rcOld.Width();
			pRect->top = 0;
			pRect->bottom = rcWorkArea.bottom;
		}
	}

	if (alignType == ALIGN_NONE)
		KillTimer(IDT_HOVER);

	CCaptionBtnDlg::OnMoving(fwSide, pRect);
}

void CIcqDlg::OnSendEmail() 
{
	CString url = "mailto:";
	url += curContact->email.c_str();
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOW);
}

void CIcqDlg::OnHomePage() 
{
	ShellExecute(NULL, _T("open"), curContact->homepage.c_str(), NULL, NULL, SW_SHOW);
}

void CIcqDlg::OnAddAsFriend() 
{
	addFriend(curContact, 0);
}

void CIcqDlg::OnMsgHistory() 
{
	CMsgMgrWnd *pWnd = new CMsgMgrWnd;
	if (pWnd->LoadFrame(IDR_MSGMGR))
		pWnd->ActivateFrame(curContact->uin);
}

void CIcqDlg::OnMessageMgr() 
{
	CMsgMgrWnd *pWnd = new CMsgMgrWnd;
	if (pWnd->LoadFrame(IDR_MSGMGR))
		pWnd->ActivateFrame();
}

void CIcqDlg::OnBroadcastMsg() 
{
	CBroadcastDlg *win = new CBroadcastDlg;
	win->Create(IDD_BROADCAST);
}

void CIcqDlg::OnOK()
{
}