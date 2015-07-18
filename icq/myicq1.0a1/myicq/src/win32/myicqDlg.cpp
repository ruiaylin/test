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
#include "udpsession.h"
#include "tcpsession.h"
#include "icqwindow.h"
#include "icqdb.h"
#include "icqconfig.h"
#include "icqplugin.h"
#include "icqgroup.h"

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
#include "GroupMsgDlg.h"
#include "GroupSearchWizard.h"
#include "GroupCreateWizard.h"
#include "GroupInfoDlg.h"
#include "icqhook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Menu item IDs
#define ID_SHOWONLINE			50001
#define ID_RENAME				50002
#define ID_REMOVE				50003
#define ID_GROUP_SEARCH			50004
#define ID_GROUP_CREATE			50005
#define ID_STATUS_AWAY			40000
#define ID_PLUGIN				60000
#define ID_SKIN_FIRST			70000

// Control IDs
#define IDC_OUTBAR				1001
#define ID_HOTKEY				1002
#define ID_TOOLTIP				1003
#define ID_NOTIFYICON			1004

// Timer IDs
#define IDT_RESEND				1001
#define IDT_KEEPALIVE			1002
#define IDT_STATUS				1003
#define IDT_BLINK				1004
#define IDT_DBLCLK				1005
#define IDT_HOVER				1006
#define IDT_AUTO_SWITCH_STATUS	1007

#define AUTO_STATUS_MONITOR_INTERVAL	(5 * 1000)

// For WSAAsyncGetHostByName
#define WM_HOSTFOUND			(WM_USER + 101)

// System tray
#define WM_NOTIFYICON			(WM_USER + 102)

// Default hot key: Ctrl + Alt + Z
#define HOTKEY_DEFAULT	MAKELONG(0x5a, HOTKEYF_ALT | HOTKEYF_CONTROL)

// Main Window's alignment
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

	CImageList il;
	il.Create(16, 16, ILC_MASK | ILC_COLOR8, NR_STATUS, 0);
	UINT id[] = {
		IDB_STATUS_ONLINE,
		IDB_STATUS_OFFLINE,
		IDB_STATUS_AWAY,
		IDB_STATUS_INVIS
	};
	CBitmap bm;
	for (int i = 0; i < NR_STATUS; ++i) {
		bm.LoadBitmap(id[i]);
		il.Add(&bm, RGB(192, 192, 192));
		bm.DeleteObject();

		statusIcons[i] = il.ExtractIcon(i);
	}

	regMode = -1;
	alignType = ALIGN_NONE;
	isHide = FALSE;

	currentMenu = NULL;
	curContact = NULL;
	currentItem = -1;

	outbarCtrl.setListener(this);
}

CIcqDlg::~CIcqDlg()
{
	for (int i = 0; i < NR_STATUS; ++i)
		DestroyIcon(statusIcons[i]);
}

/*
 * Interface implementation
 */
void *CIcqDlg::getFaceIcon(int face, int status)
{
	return (void *) getApp()->getLargeFace(face, status);
}

void CIcqDlg::popupMsg(void *icon, const char *text, uint32 t)
{
	new CNotifyWnd((HICON) icon, text, t);
}

void CIcqDlg::exitGroup(IcqGroup *g)
{
	outbarCtrl.removeGroup(g->id);

	IcqLink::exitGroup(g);
}


void CIcqDlg::minimize()
{
	CRect from, to;

	GetWindowRect(from);
	SystemParametersInfo(SPI_GETWORKAREA, 0, to, 0);
	to.left = to.right - from.Width();
	to.top = to.bottom - GetSystemMetrics(SM_CYCAPTION);

	DrawAnimatedRects(m_hWnd, IDANI_CAPTION, from, to);
	ShowWindow(SW_HIDE);
}

void CIcqDlg::restore()
{
	ShowWindow(SW_NORMAL);
	SetForegroundWindow();
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
		// the 'minus'(-) button is clicked
		minimize();

	} else if (button == 1) {
		// the 'plus'(+) button is clicked
		CGfxPopupMenu menu;
		menu.CreatePopupMenu();

		int id = 1000;
		menu.insertMenu(id, options.skin.empty() ? IDB_CHECK : IDB_FOLDER, IDS_SKIN_RESTORE);

		CFileFind finder;
		BOOL cont = finder.FindFile(getApp()->rootDir + "skins\\*");
		while (cont) {
			cont = finder.FindNextFile();
			if (!finder.IsDots() && finder.IsDirectory()) {
				int bmpID = (finder.GetFileTitle() == options.skin.c_str() ? IDB_CHECK : IDB_FOLDER);
				menu.insertMenu(++id, bmpID, finder.GetFileTitle());
			}
		}

		CPoint pt;
		GetCursorPos(&pt);
		id = menu.TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this);
		if (id == 0)
			return;

		ShowWindow(SW_MINIMIZE);

		if (id == 1000) {
			// Original skin
			outbarCtrl.setSkin(NULL);
			options.skin = "";
		} else {
			// Load new skin
			CString str;
			menu.GetMenuString(id, str, 0);
			if (skin.load(getApp()->rootDir + "skins\\" + str))
				outbarCtrl.setSkin(&skin);

			options.skin = str;
		}

		IcqDB::saveOptions(options);
		restore();
	}
}

/*
 * Under normal cases, this function is called immediately after connect() is called,
 * but if we are using proxy, it may be delayed
 */
void CIcqDlg::onConnect(bool success)
{
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win) {
		((CRegWizard *) win)->onConnect(success);
		return;
	}

	if (success)
		getUdpSession()->login(myInfo.passwd.c_str(), pendingStatus);
	else {
		myInfo.status = STATUS_OFFLINE;
		updateStatusIcon();
		logout();
	}
}

/*
 * Add a contact to the folder
 */
IcqContact *CIcqDlg::addContact(QID &qid, int folder)
{
	outbarCtrl.removeContact(qid);

	IcqContact *c = findContact(qid);
	if (!c) {
		c = new IcqContact;
		c->qid = qid;

		// For now, we do not know the contact's nick name, just use the uin
		c->nick = qid.toString();
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

	SetTimer(IDT_RESEND, SEND_TIMEOUT * 1000, NULL);
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
		outbarCtrl.update(c->qid, TRUE);
	}
}

void CIcqDlg::addFriend(IcqContact *c, int folder)
{
	if (folder < 0)
		folder = outbarCtrl.getSelFolder();
	c->type = CONTACT_FRIEND;

	new CSysMsgDlg(c->qid);
}

BOOL CIcqDlg::delFriend(IcqContact *c, int folder)
{
	CString str;
	str.Format(IDS_PROMPT_DEL_FRIEND, c->qid.toString());
	if (AfxMessageBox(str, MB_YESNO) != IDYES)
		return FALSE;

	// Note that the parentheses are necessary, since the destructor must be called to
	// remove the dialog from the window list.
	{
		CDelFriendDlg dlg(c->qid);
		if (dlg.DoModal() != IDOK)
			return FALSE;
	}

	int n = outbarCtrl.getFolderCount();
	if (folder == n - 1 || folder < 0)
		destroyContact(c->qid);

	if (folder == n - 2 || folder < 0)
		IcqDB::delContact(c->qid);
	outbarCtrl.removeContact(c->qid);
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
		getUdpSession()->connect(inet_ntoa(addr), options.port);
	else {
		logout();
		updateStatusIcon();
	}
}

/*
 * Notify the user that a friend is online
 */
void CIcqDlg::onlineNotify(IcqContact *c)
{
	if (options.flags.test(UF_ONLINE_NOTIFY)) {
		options.playSound(SOUND_ONLINE, c);
		outbarCtrl.blinkText(c->qid);

		if (c->flags.test(CF_OPTION_POPUP_ONLINE)) {
			CString str;
			str.Format(IDS_NOTIFY_ONLINE, c->nick.c_str(), c->qid.toString());
			new CNotifyWnd(getApp()->getLargeFace(c->face), str);
		}
	}
}

/*
 * Pop up the main menu
 */
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
	if (myInfo.qid.uin != ADMIN_UIN)
		menu.DeleteMenu(ID_BROADCAST_MSG, 0);
	else
		menu.modifyMenu(ID_BROADCAST_MSG, iconSysMsg);

	// MyICQ main logo
	menu.setVertBitmap(IDB_VERT);

	currentMenu = &menu;
	menu.TrackPopupMenu(nFlags, pt.x, pt.y, this);
	currentMenu = NULL;
}

/*
 * Pop up the status menu
 */
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

/*
 * Modify the system tray icon according to our current status, and stop animating
 */
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

	HICON icon = statusIcons[myInfo.status];
	m_btnStatus.SetIcon(icon);
	sysTray.setIcon(icon);
}

/*
 * Start icon animation
 */
void CIcqDlg::animateStatus()
{
	SetTimer(IDT_STATUS, 250, NULL);
	OnTimer(IDT_STATUS);
}

/* 
 * Show or hide the main window animatedly
 */
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
		SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_NOACTIVATE);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
		Sleep(20);
	}
	SetWindowPos(NULL, xEnd, yEnd, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

	isHide = hide;
	if (!isHide)
		SetTimer(IDT_HOVER, 50, NULL);
}

/*
 * Add a message to the un-received message list, we will fetch it later
 */
void CIcqDlg::addPendingMsg(IcqMsg *msg)
{
	if (msgList.empty())
		SetTimer(IDT_BLINK, 400, NULL);

	if (!msg->id) {
		if (!msg->isSysMsg() && !findPendingMsg(&msg->qid))
			outbarCtrl.blinkImage(msg->qid);
	} else
		outbarCtrl.blinkImage(msg->id);

	IcqLink::addPendingMsg(msg);
}

// Callback functions from ServerSession

void CIcqDlg::onNewUINReply(QID &qid)
{
	myInfo.qid = qid;
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win)
		((CRegWizard *) win)->onNewUINReply(qid);
}

void CIcqDlg::onContactListReply(QIDList &l)
{
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win) {
		qidList = l;
		((CRegWizard *) win)->onContactListReply();
	}
}

void CIcqDlg::onRemoteContactList(const char *domain, QIDList &l)
{
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win) {
		qidList.splice(qidList.end(), l);
		((CRegWizard *) win)->onRemoteContactList(domain);
	}
}

void CIcqDlg::onLoginReply(uint16 error, uint32 ip)
{
	IcqWindow *win = findWindow(WIN_REG_WIZARD);
	if (win) {
		((CRegWizard *) win)->onLoginReply(error);
		return;
	}

	if (error != LOGIN_SUCCESS) {
		KillTimer(IDT_RESEND);
		AfxMessageBox(IDS_WRONG_PASSWD);
		return;
	}

	ourIP = ip;

	SetTimer(IDT_KEEPALIVE, KEEPALIVE_TIMEOUT * 1000, NULL);

	myInfo.status = pendingStatus;
	updateStatusIcon();

	restore();

	if (regMode == 0)			// New UIN
		getUdpSession()->modifyUser(&myInfo);
	else if (regMode == 1) {	// Existing UIN
		// Fetch back our personal information from the server
		getUdpSession()->updateUser();

		// Update all contacts' information
		while (!qidList.empty()) {
			QID qid = qidList.front();
			qidList.pop_front();
			getUdpSession()->updateContact(qid);
		}
	}
	regMode = -1;
}

void CIcqDlg::onUpdateContactReply(IcqContact *info)
{
	IcqLink::onUpdateContactReply(info);

	IcqContact *c = findContact(info->qid);
	if (c) {
		IcqDB::saveContact(*c);
		outbarCtrl.update(c->qid);
	}

	uint16 seq = getUdpSession()->ackseq;
	IcqWindow *win = findWindow(WIN_VIEW_DETAIL, NULL, seq);
	if (win)
		((CViewDetailDlg *) win)->onUpdateContactReply(info);
}

void CIcqDlg::onUpdateUserReply()
{
	IcqDB::saveUser(myInfo);
}

void CIcqDlg::onUserOnline(QID &qid, uint32 status, uint32 ip, uint16 port, uint32 realIP, bool multi)
{
	if (status != STATUS_ONLINE && status != STATUS_AWAY)
		return;

	IcqContact *c = findContact(qid, CONTACT_FRIEND);
	if (!c)
		c = addContact(qid);

	c->ip = ip;
	c->port = port;
	c->realIP = realIP;		// The real IP behind a firewall, or 0 if no firewall
	c->status = status;
	outbarCtrl.update(qid, TRUE);

	if (c->flags.test(CF_OPTION_INVISIBLE))
		OnStatusInvis();

	// Send a greeting
	if (c->flags.test(CF_OPTION_GREETING) && !c->greeting.empty())
		sendMessage(MSG_TEXT, qid, c->greeting.c_str());
	
	if (!multi)
		onlineNotify(c);
}

void CIcqDlg::onUserOffline(QID &qid)
{
	IcqLink::onUserOffline(qid);

	outbarCtrl.update(qid, TRUE);

	IcqContact *c = findContact(qid);
	if (c && c->flags.test(CF_OPTION_POPUP_OFFLINE)) {
		CString str;
		str.Format(IDS_NOTIFY_OFFLINE, c->nick.c_str(), c->qid.toString());
		new CNotifyWnd(getApp()->getLargeFace(c->face), str);
	}
}

void CIcqDlg::onStatusChanged(QID &qid, uint32 status)
{
	if (status != STATUS_ONLINE && status != STATUS_AWAY)
		return;

	IcqContact *c = findContact(qid);
	if (c) {
		if (c->status != STATUS_ONLINE && status == STATUS_ONLINE)
			onlineNotify(c);
		c->status = status;
		outbarCtrl.update(qid, TRUE);
	}
}

void CIcqDlg::onRecvMessage(IcqMsg *msg)
{
	uint8 type = msg->type;
	QID &from = msg->qid;
	BOOL relay = (msg->flags & MF_RELAY);

	if (type == MSG_AUTH_ACCEPTED) {
		// Add a new contact since we are authorized
		getUdpSession()->updateContact(from);
		addContact(from);
	}

	IcqContact *c = findContact(from);

	if (!c && !msg->isSysMsg()) {
		// It is a message from a stranger, add it to the strangers folder
		int n = outbarCtrl.getFolderCount();
		c = addContact(from, n - 2);
	}

	if (msg->isSysMsg()) {
		// It is a system message
		options.playSound(SOUND_SYSMSG);
		addPendingMsg(msg);
		IcqDB::saveMsg(*msg);
		return;
	}
	if (!c)
		return;

	if (c->status == STATUS_OFFLINE && !relay) {
		// Wow, we have received a message directly from an offline user!
		// This is because he/she is invisible now, just make it visible;-)
		c->status = STATUS_ONLINE;
		outbarCtrl.update(c->qid, TRUE);
	}

	options.playSound(SOUND_MSG, c);
	if (type == MSG_TCP_REQUEST) {
		addPendingMsg(msg);
		return;
	}
	IcqDB::saveMsg(*msg);

	// Auto reply
	if ((myInfo.status == STATUS_AWAY) &&
		options.flags.test(UF_AUTO_REPLY) &&
		!options.autoReplyText.empty() &&
		type != MSG_AUTO_REPLY)
		sendMessage(MSG_AUTO_REPLY, from, options.autoReplyText.c_str());

	IcqWindow *win;
	if (c->flags.test(CF_DIALOGMODE)) {
		// In dialog mode, show the 'Send Message' dialog
		win = findWindow(WIN_SEND_MESSAGE, &from);
		if (!win)
			win = new CSendMsgDlg(c);
		((CSendMsgDlg *) win)->onRecvMessage(*msg);
		delete msg;

	} else {
		// Enqueue it into the un-received message list
		addPendingMsg(msg);

		win = findWindow(WIN_VIEW_MESSAGE, &from);

		// If the 'View Message' dialog is already open, notify it that
		// a new message is coming. otherwise, pop it up
		if (win)
			((CViewMsgDlg *) win)->onRecvMessage();
		else if (options.flags.test(UF_AUTO_POPUP_MSG))
			new CViewMsgDlg(c);
	}
}

void CIcqDlg::onUserFound(PtrList *l)
{
	uint16 seq = getUdpSession()->ackseq;
	IcqWindow *win = findWindow(WIN_SEARCH_WIZARD, NULL, seq);
	if (win)
		((CSearchWizard *) win)->onUserFound(l);
}

void CIcqDlg::onAddFriendReply(QID &qid, uint8 result)
{
	uint16 seq = getUdpSession()->ackseq;
	IcqWindow *win;
	if (win = findWindow(WIN_SEARCH_WIZARD, NULL, seq))
		((CSearchWizard *) win)->onAddFriendReply(result);
	else if ((win = findWindow(WIN_SYS_MESSAGE, NULL, seq)) ||
			(win = findWindow(WIN_ADD_FRIEND, NULL, seq)))
		((CSysMsgDlg *) win)->onAddFriendReply(result);
	else
		return;

	if (result == ADD_FRIEND_ACCEPTED) {
		getUdpSession()->updateContact(qid);
		addContact(qid);
	}
}

void CIcqDlg::onServerListReply(SERVER_INFO info[], int n)
{
	IcqWindow *win = findWindow(WIN_SEARCH_WIZARD);
	if (win)
		((CSearchWizard *) win)->onServerListReply(info, n);
}

void CIcqDlg::onCreateGroupReply(uint32 id)
{
	IcqWindow *win = findWindow(WIN_GROUP_CREATE_WIZARD);
	if (!win)
		return;

	CGroupCreateWizard *wiz = (CGroupCreateWizard *) win;
	wiz->onCreateGroupReply(id);

	IcqGroup *g = IcqLink::createGroup(wiz->groupPlugin, id);
	if (g) {
		g->name = wiz->basicPage.m_groupName;

		g->addMember(myInfo.qid.uin, myInfo.face, myInfo.nick.c_str());
		outbarCtrl.addGroup(g);
	}
}

void CIcqDlg::onGroupListReply(PtrList &l)
{
	IcqWindow *win = findWindow(WIN_GROUP_SEARCH_WIZARD);
	if (win)
		((CGroupSearchWizard *) win)->onGroupListReply(l);
}

void CIcqDlg::onSearchGroupReply(GROUP_INFO *info)
{
	IcqWindow *win = findWindow(WIN_GROUP_SEARCH_WIZARD);
	if (win)
		((CGroupSearchWizard *) win)->onSearchGroupReply(info);
}

void CIcqDlg::onEnterGroupReply(uint32 id, uint16 error)
{
	IcqWindow *win = findWindow(WIN_GROUP_SEARCH_WIZARD);
	if (!win)
		return;

	CGroupSearchWizard *wiz = (CGroupSearchWizard *) win;
	wiz->onEnterGroupReply(id, error);

	if (error == GROUP_ERROR_SUCCESS) {
		GROUP_INFO *info = wiz->resultPage.getResult();
		GROUP_TYPE_INFO *type = icqLink->getGroupTypeInfo(info->type);
		IcqGroup *g = IcqLink::createGroup(type->name.c_str(), id);
		if (g) {
			g->name = info->name;
			outbarCtrl.addGroup(g);
		}
	}
}

void CIcqDlg::onGroupMessage(IcqMsg *msg)
{
	IcqGroup *g = findGroup(msg->id);
	if (!g)
		return;

	CGroupMsgDlg *dlg = NULL;
	GroupWindow *win = findGroupWindow(msg->id);

	if (win)
		dlg = (CGroupMsgDlg *) win;
	else if (g->msgOption == GROUP_OPTION_MSG_POPUP)
		dlg = new CGroupMsgDlg(g);

	if (dlg) {
		dlg->onRecvMessage(*msg);
		delete msg;
	} else {
		addPendingMsg(msg);

		options.playSound(SOUND_MSG);
	}
}

/*
 * Default options that a new registered user should have.
 */
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

	// By default, there are three folders in the OutBar
	outbarCtrl.removeAllFolders();

	str.LoadString(IDS_GROUP_FRIEND);
	outbarCtrl.addFolder(str);
	str.LoadString(IDS_MY_GROUP);
	outbarCtrl.addFolder(str);
	str.LoadString(IDS_GROUP_STRANGER);
	outbarCtrl.addFolder(str);
	str.LoadString(IDS_GROUP_BLACKLIST);
	outbarCtrl.addFolder(str);
	outbarCtrl.setSelFolder(0);
}

void CIcqDlg::resolveHost()
{
	const char *host = options.host.c_str();

	if (options.flags.test(UF_USE_PROXY) && !options.proxy[options.proxyType].resolve)
		getUdpSession()->connect(host, options.port);
	else {
		struct in_addr addr;
		if ((addr.s_addr = inet_addr(host)) != INADDR_NONE)
			onHostFound(addr);
		else
			getHostByName(host);
	}
}

void CIcqDlg::setHotKey()
{
	uint32 hotKey = (options.flags.test(UF_DEFAULT_HOTKEY) ?
		HOTKEY_DEFAULT : options.hotKey);
	UINT vk = LOWORD(hotKey);
	hotKey = HIWORD(hotKey);
	UINT mod = 0;
	if (hotKey & HOTKEYF_ALT)
		mod |= MOD_ALT;
	if (hotKey & HOTKEYF_CONTROL)
		mod |= MOD_CONTROL;
	if (hotKey & HOTKEYF_SHIFT)
		mod |= MOD_SHIFT;
	if (!RegisterHotKey(m_hWnd, ID_HOTKEY, mod, vk))
		AfxMessageBox(IDS_FAILED_HOTKEY);
}

/*
 * Initialize GUI elements according to the current options
 */
void CIcqDlg::initGUI()
{
	SetWindowText(myInfo.qid.toString());

#ifndef _DEBUG
	SetWindowPos(options.flags.test(UF_ALWAYS_ON_TOP) ? &wndTopMost : &wndNoTopMost,
		0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
#endif

	sysTray.destroy();
	if (options.flags.test(UF_TASKBAR_ICON)) {
		sysTray.create(this, ID_NOTIFYICON, WM_NOTIFYICON);

		CString tip;
		tip.Format("%s %s", AfxGetAppName(), myInfo.qid.toString());
		sysTray.setToolTip(tip);
		sysTray.setIcon(statusIcons[0]);
	}

	// Auto status
	KillTimer(IDT_AUTO_SWITCH_STATUS);
	IcqHook::uninstall();
	if (options.flags.test(UF_AUTO_SWITCH_STATUS) && IcqHook::install())
		SetTimer(IDT_AUTO_SWITCH_STATUS, AUTO_STATUS_MONITOR_INTERVAL, NULL);

	UnregisterHotKey(m_hWnd, ID_HOTKEY);
	if (options.flags.test(UF_USE_HOTKEY))
		setHotKey();

	outbarCtrl.setShowOnlineOnly(options.flags.test(UF_SHOW_ONLINE));

	if (options.skin.empty())
		outbarCtrl.setSkin(NULL);
	else if (skin.load(getApp()->rootDir + "skins\\" + options.skin.c_str()))
		outbarCtrl.setSkin(&skin);
}

BOOL CIcqDlg::doRegWizard()
{
	destroyUser();

	SetTimer(IDT_RESEND, SEND_TIMEOUT * 1000, NULL);

	// Note that the parentheses are necessary, since the destructor must be called to
	// remove the dialog from the window list.
	{
		CRegWizard wiz;
		wiz.DoModal();
		if (!wiz.isFinished)
			return FALSE;

		wiz.getData(&myInfo, &options);
		regMode = wiz.modeDlg.m_mode;
	}

	IcqConfig::addUser(myInfo.qid);
	CString dir;
	getApp()->getUserDir(dir, myInfo.qid);
	IcqDB::setDir(dir);

	setDefaultOptions();
	IcqDB::saveOptions(options);
	IcqDB::saveUser(myInfo);

	if (regMode == 1) {
		// Existing UIN.
		// Correct the contact list to be updated.
		QIDList::iterator it;
		for (it = qidList.begin(); it != qidList.end(); ++it)
			addContact(*it);
	}
	saveGroupInfo();
	outbarCtrl.Invalidate();

	initUser();
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

	myInfo.qid = loginDlg.m_qid;
	CString dir;
	getApp()->getUserDir(dir, myInfo.qid);
	IcqDB::setDir(dir);

	if (!IcqDB::loadOptions(options) ||
		!IcqDB::loadUser(myInfo) ||
		!loadGroupInfo()) {
		if (AfxMessageBox(IDS_PROMPT_DB_CORRUPTED, MB_YESNO) == IDYES)
			return doRegWizard();
		return FALSE;
	}

	// Verify password
	CString passwd = loginDlg.m_passwd;
	while (passwd != myInfo.passwd.c_str()) {
		CLoginAgainDlg againDlg(loginDlg.m_qid);
		if (againDlg.DoModal() != IDOK)
			return FALSE;

		passwd = againDlg.m_passwd;
	}

	initUser();
	login(options.flags.test(UF_LOGIN_INVIS) ? STATUS_INVIS : STATUS_ONLINE);
	return TRUE;
}

void CIcqDlg::destroyUser()
{
	IcqLink::destroyUser();

	UnregisterHotKey(m_hWnd, ID_HOTKEY);
	sysTray.destroy();
	regMode = -1;
}

/*
 * Fetch back a contact message from the un-received message list
 * qid == NULL means that we are only interested in system message
 */
IcqMsg *CIcqDlg::fetchMsg(QID *qid)
{
	IcqMsg *msg = findPendingMsg(qid);
	if (msg)
		msgList.remove(msg);

	if (!findPendingMsg(qid)) {
		// There are no messages left in the queue
		if (qid)
			outbarCtrl.blinkImage(*qid, FALSE);
		else
			m_btnSysMsg.SetIcon(iconSysMsg);
	}
	if (msgList.empty()) {
		KillTimer(IDT_BLINK);
		updateStatusIcon();
	}
	return msg;
}

void CIcqDlg::fetchMsg(PtrList &l, uint32 id)
{
	IcqMsg *msg = findPendingMsg(id);

	while (msg) {
		msgList.remove(msg);
		l.push_back(msg);

		msg = findPendingMsg(id);
	}

	outbarCtrl.blinkImage(id, FALSE);

	if (msgList.empty()) {
		KillTimer(IDT_BLINK);
		updateStatusIcon();
	}
}

/*
 * Show the 'System Options' dialog
 */
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
			getUdpSession()->changeStatus(myInfo.status);
		}
		updateStatusIcon();
	}
}

void CIcqDlg::onAutoStatus()
{
	DWORD thisTick = GetTickCount();
	DWORD d = GetTickCount() - IcqHook::getLastTick();

	if (options.autoStatusTime > 0 && d >= options.autoStatusTime * 60000) {
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
	} else if (myInfo.status == STATUS_AWAY &&
		options.flags.test(UF_AUTO_CANCEL_AWAY) &&
		d <= AUTO_STATUS_MONITOR_INTERVAL) {
		OnStatusOnline();
	}
}

/*
 * Called when the system tray icon is double clicked
 */
void CIcqDlg::onNotifyDblClk()
{
	if (msgList.empty()) {
		// If there are no new messages, just show the main window
		if (isHide) {
			hideWindow(FALSE);
			SetTimer(IDT_HOVER, 5000, NULL);
		} else
			restore();

	} else {
		IcqMsg *msg = (IcqMsg *) msgList.front();

		if (msg->isSysMsg())
			OnSysMessage();
		else if (!msg->id) {
			IcqContact *c = findContact(msg->qid);
			if (c)
				onContactDoubleClicked(c);
		} else {
			IcqGroup *g = findGroup(msg->id);
			if (g)
				onGroupDoubleClicked(g);
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
	
	if (!IcqDB::loadGroupInfo(outbarCtrl) || outbarCtrl.getFolderCount() < 4)
		return FALSE;
	return TRUE;
}

/*
 * This function should be called when a new user logins
 */
void CIcqDlg::initUser()
{
	CString dir;
	getApp()->getUserDir(dir, myInfo.qid);
	IcqLink::initUser(dir);

	initGUI();
}

/*
 * Search a certain directory to load all plugins
 */
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
		GETPLUGININFO getPluginInfo = (GETPLUGININFO) GetProcAddress(module, "getPluginInfo");
		if (!getPluginInfo) {
			FreeLibrary(module);
			continue;
		}

		PluginInfo info;
		getPluginInfo(&info);

		IcqPlugin *plugin;
		if (info.type == ICQ_PLUGIN_NET) {
			NetPlugin *p = new NetPlugin;
			p->createSession = (CREATESESSION) GetProcAddress(module, "createSession");
			plugin = p;
		} else if (info.type == ICQ_PLUGIN_TOY) {
			ToyPlugin *p = new ToyPlugin;
			p->init = (INIT) GetProcAddress(module, "init");
			p->configure = (CONFIGURE) GetProcAddress(module, "configure");
			p->about = (ABOUT) GetProcAddress(module, "about");
			plugin = p;
		} else
			continue;

		plugin->name = finder.GetFileTitle();
		plugin->info = info;
		PluginFactory::registerPlugin(plugin);
	}

	// Next, register the exe plugin
	dir = getApp()->rootDir + "plugins\\*.ini";
	cont = finder.FindFile(dir);
	while (cont) {
		cont = finder.FindNextFile();
		registerExePlugin(finder.GetFilePath(), finder.GetFileTitle());
	}
	return TRUE;
}


// OutBarListener...

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
	if (!c)
		return;

	int fromFolder = outbarCtrl.getSelFolder();
	int n = outbarCtrl.getFolderCount();

	if (toFolder == n - 3)
		return;

	if (fromFolder >= n - 2 && toFolder < n - 2)
		addFriend(c, toFolder);
	else if (fromFolder < n - 2 && toFolder >= n - 2)
		delFriend(c, toFolder);
	else {
		if (toFolder == n - 1)
			c->type = CONTACT_IGNORE;
		else if (toFolder == n - 2)
			c->type = CONTACT_STRANGER;

		outbarCtrl.removeContact(c->qid);
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
	currentItem = item;
	curContact = outbarCtrl.contact(item);

	CGfxPopupMenu menu;
	CMenu tmp;

	int sel = outbarCtrl.getSelFolder();
	int n = outbarCtrl.getFolderCount();
	if (sel == n - 1)
		tmp.LoadMenu(IDR_IGNORE);
	else if (sel == n - 2)
		tmp.LoadMenu(IDR_STRANGER);
	else if (sel == n - 3)
		tmp.LoadMenu(IDR_GROUP);
	else
		tmp.LoadMenu(IDR_CONTACT);

	menu.Attach(*tmp.GetSubMenu(0));

	if (sel < n - 3 && curContact->status != STATUS_OFFLINE) {
		// Add plugin menu items
		UINT id = ID_PLUGIN;
		PtrList &l = PluginFactory::getPluginList();
		PtrList::iterator it;
		for (it = l.begin(); it != l.end(); ++it) {
			IcqPlugin *p = (IcqPlugin *) *it;
			int type = p->info.type;
			if (type == ICQ_PLUGIN_NET || type == ICQ_PLUGIN_EXE) {
				menu.AppendMenu(0, id, p->info.name.c_str());
				menu.modifyMenu(id++, (HICON) p->info.icon);
			}
		}
	}

	menu.modifyMenu(ID_SEND_MSG, IDB_SEND_MSG);
	menu.modifyMenu(ID_SEND_EMAIL, IDB_SEND_EMAIL);
	menu.modifyMenu(ID_HOMEPAGE, IDB_HOMEPAGE);
	menu.modifyMenu(ID_MSG_HISTORY, IDB_MSG_HISTORY);
	menu.modifyMenu(ID_VIEW_DETAIL, IDB_VIEW_DETAIL);
	menu.modifyMenu(ID_ADD_AS_FRIEND, IDB_ADD);

	menu.modifyMenu(ID_GROUP_DETAIL, IDB_VIEW_DETAIL);
	menu.modifyMenu(ID_GROUP_SEND_MSG, IDB_SEND_MSG);
	menu.modifyMenu(ID_GROUP_EXIT, IDB_EXIT);

	// Disable menu items
	if (myInfo.status == STATUS_OFFLINE)
		menu.EnableMenuItem(ID_SEND_MSG, MF_GRAYED);

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

	int n = outbarCtrl.getFolderCount();
	int sel = outbarCtrl.getSelFolder();
	if (sel == n - 3) {
		menu.insertMenu(ID_GROUP_SEARCH, IDB_ADD, IDS_SEARCH_GROUP, 0);
		menu.insertMenu(ID_GROUP_CREATE, IDB_GROUP, IDS_CREATE_GROUP, 1);
		menu.InsertMenu(2, MF_SEPARATOR | MF_BYPOSITION);
	}

	menu.modifyMenu(ID_SMALLICON, IDB_SMALLICON);
	menu.modifyMenu(ID_LARGEICON, IDB_LARGEICON);
	menu.modifyMenu(ID_ADDGROUP, IDB_ADDGROUP);
	menu.modifyMenu(ID_FONTCOLOR, IDB_FONTCOLOR);
	menu.modifyMenu(ID_BGCOLOR, IDB_BACKGROUND);
	menu.insertMenu(ID_SHOWONLINE, IDB_SHOWONLINE,
		outbarCtrl.showOnlineOnly() ? IDS_SHOWALL : IDS_SHOWONLINE);

	if (obj == OutBarCtrl::HT_ITEM) {
		menu.AppendMenu(MF_SEPARATOR);
		menu.insertMenu(ID_REMOVE, IDB_REMOVEITEM, IDS_REMOVEITEM);
		menu.insertMenu(ID_RENAME, IDB_RENAME, IDS_RENAMEITEM);

	} else if (obj == OutBarCtrl::HT_FOLDER) {
		int n = outbarCtrl.getFolderCount();
		if (index > 0 && index < n - 2) {
			menu.AppendMenu(MF_SEPARATOR);
			menu.insertMenu(ID_REMOVE, IDB_REMOVEGROUP, IDS_REMOVEGROUP);
			menu.insertMenu(ID_RENAME, IDB_RENAME, IDS_RENAMEGROUP);
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
	IcqContact *c = outbarCtrl.contact(item);
	if (c)
		onContactDoubleClicked(c);
	else
		onGroupDoubleClicked(outbarCtrl.group(item));
}

void CIcqDlg::onContactDoubleClicked(IcqContact *c)
{
	int sel = outbarCtrl.getSelFolder();
	int n = outbarCtrl.getFolderCount();
	IcqMsg *msg = fetchMsg(&c->qid);

	if (msg && msg->type == MSG_TCP_REQUEST) {
		if (sel < n - 2) {
			new CSendRequestDlg(msg);
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

void CIcqDlg::onGroupDoubleClicked(IcqGroup *g)
{
	CGroupMsgDlg *dlg = NULL;
	GroupWindow *win = findGroupWindow(g->id);

	if (win)
		dlg = (CGroupMsgDlg *) win;
	else
		dlg = new CGroupMsgDlg(g);

	PtrList l;
	fetchMsg(l, g->id);

	PtrList::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		IcqMsg *msg = (IcqMsg *) *it;
		dlg->onRecvMessage(*msg);
		delete msg;
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
	DDX_Control(pDX, IDC_HOMEPAGE, m_btnHome);
	DDX_Control(pDX, IDC_ABOUT, m_btnAbout);
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
	ON_COMMAND(ID_SMALLICON, OnSmallIcon)
	ON_UPDATE_COMMAND_UI(ID_SMALLICON, OnUpdateSmallIcon)
	ON_COMMAND(ID_LARGEICON, OnLargeIcon)
	ON_UPDATE_COMMAND_UI(ID_LARGEICON, OnUpdateLargeIcon)
	ON_COMMAND(ID_FONTCOLOR, OnFontColor)
	ON_COMMAND(ID_BGCOLOR, OnBgColor)
	ON_COMMAND(ID_ADDGROUP, OnAddGroup)
	ON_UPDATE_COMMAND_UI(ID_ADDGROUP, OnUpdateAddGroup)
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_COMMAND(ID_MODIFY_INFO, OnModifyInfo)
	ON_COMMAND(ID_SHOWONLINE, OnShowOnline)
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
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_WM_DESTROY()
	ON_WM_MOVING()
	ON_COMMAND(ID_SEND_EMAIL, OnSendEmail)
	ON_COMMAND(ID_HOMEPAGE, OnHomePage)
	ON_COMMAND(ID_ADD_AS_FRIEND, OnAddAsFriend)
	ON_COMMAND(ID_MSG_HISTORY, OnMsgHistory)
	ON_COMMAND(ID_MESSAGE_MGR, OnMessageMgr)
	ON_COMMAND(ID_BROADCAST_MSG, OnBroadcastMsg)
	ON_BN_CLICKED(IDC_HOMEPAGE, OnHomepage)
	ON_WM_CLOSE()
	ON_COMMAND(ID_GROUP_DETAIL, OnGroupDetail)
	ON_COMMAND(ID_GROUP_SEND_MSG, OnGroupSendMsg)
	ON_COMMAND(ID_GROUP_SEARCH, OnGroupSearch)
	ON_COMMAND(ID_GROUP_CREATE, OnGroupCreate)
	ON_COMMAND(ID_GROUP_EXIT, OnGroupExit)
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

	// Add a tool tip to the whole area of the main window
	toolTip.Create(this);
	CRect rc;
	GetWindowRect(rc);
	ScreenToClient(rc);
	toolTip.AddTool(this, LPSTR_TEXTCALLBACK, rc, ID_TOOLTIP);

	m_btnSearch.SetIcon(IDI_FIND);
	m_btnSysMsg.SetIcon(iconSysMsg);
	m_btnAbout.SetIcon(IDI_ABOUT);
	m_btnHome.SetIcon(IDI_HOMEPAGE);
	m_btnMain.SetFlat(FALSE, FALSE);

	// Add button tool tip text
	m_btnStatus.SetTooltipText(IDS_CHANGE_STATUS);
	m_btnMain.SetTooltipText(IDS_MAINMENU);
	m_btnSearch.SetTooltipText(IDS_SEARCH);
	m_btnSysMsg.SetTooltipText(IDS_SYSMSG);
	m_btnAbout.SetTooltipText(IDS_ABOUT);
	m_btnHome.SetTooltipText(IDS_MYICQ_HOMEPAGE);

	outbarCtrl.CreateEx(WS_EX_CLIENTEDGE, NULL, NULL, WS_VISIBLE | WS_CHILD,
		CRect(0, 0, 0, 0), this, IDC_OUTBAR);

	loadPlugins();

	outbarCtrl.SetFocus();
	return FALSE;
}

/*
 * Adjust all of the child controls according to the geometry of the main window
 */
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

	m_btnHome.GetWindowRect(rc);
	ScreenToClient(rc);
	y -= rc.Height() + 2;
	m_btnHome.MoveWindow(rc.left, y, rc.Width(), rc.Height(), FALSE);

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

/*
 * Set maximum and minimum window size limitations
 */
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
		// Fake that we are hitting the window caption to drag it
		return HTCAPTION;

	// To see whether the window should be shown animatedly if it is hidden.

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
	if (pMsg->hwnd == m_hWnd)
		toolTip.RelayEvent(pMsg);

	return CCaptionBtnDlg::PreTranslateMessage(pMsg);
}

void CIcqDlg::OnSendMessage() 
{
	IcqContact *c = curContact;
	IcqMsg *msg = fetchMsg(&c->qid);

	if (myInfo.status == STATUS_OFFLINE && !msg)
		return;

	IcqWindow *win;
	if (c->flags.test(CF_DIALOGMODE) || !msg) {
		outbarCtrl.blinkImage(c->qid, FALSE);

		CSendMsgDlg *dlg;
		win = findWindow(WIN_SEND_MESSAGE, &c->qid);
		if (!win)
			dlg = new CSendMsgDlg(c);
		else
			dlg = (CSendMsgDlg *) win;

		dlg->setTop();

		if (c->flags.test(CF_DIALOGMODE)) {
			while (msg) {
				dlg->onRecvMessage(*msg);
				delete msg;
				msg = fetchMsg(&c->qid);
			}
		}
	} else {
		// Fake that we have not fetched it.
		msgList.push_front(msg);

		CViewMsgDlg *dlg;
		win = findWindow(WIN_VIEW_MESSAGE, &c->qid);
		if (!win)
			dlg = new CViewMsgDlg(c);
		else
			dlg = (CViewMsgDlg *) win;

		dlg->setTop();
	}
}

void CIcqDlg::OnViewDetail() 
{
	new CViewDetailDlg(curContact->qid);
}

void CIcqDlg::OnFindContact() 
{
	if (myInfo.status == STATUS_OFFLINE) {
		AfxMessageBox(IDS_SEARCH_FAILED);
		return;
	}
	
	if (outbarCtrl.getSelFolder() == outbarCtrl.getFolderCount() - 3) {
		if (!findWindow(WIN_GROUP_SEARCH_WIZARD))
			new CGroupSearchWizard;
	} else {
		if (!findWindow(WIN_SEARCH_WIZARD))
			new CSearchWizard;
	}
}

void CIcqDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_RESEND)
		getUdpSession()->checkSendQueue();
	else if (nIDEvent == IDT_KEEPALIVE)
		getUdpSession()->sendKeepAlive();
	else if (nIDEvent == IDT_STATUS) {
		static int frame = 0;

		HICON icon = statusIcons[frame];
		m_btnStatus.SetIcon(icon);
		sysTray.setIcon(icon);
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
		} else if (!msg->id) {
			IcqContact *c = findContact(msg->qid);
			if (c)
				icon = (frame ? getApp()->getSmallFace(c->face) : iconBlank);
		} else {
			IcqGroup *g = findGroup(msg->id);
			if (g)
				icon = (frame ? getApp()->getGroupIcon(g) : iconBlank);
		}
		sysTray.setIcon(icon);

	} else if (nIDEvent == IDT_DBLCLK) {
		KillTimer(nIDEvent);

		CPoint pt;
		GetCursorPos(&pt);
		SetForegroundWindow();
		popupStatusMenu(TPM_BOTTOMALIGN | TPM_RIGHTALIGN, pt);

	} else if (nIDEvent == IDT_HOVER) {
		// If the mouse is moved out of the window, hide it
		CPoint pt;
		GetCursorPos(&pt);
		CRect rc;
		GetWindowRect(rc);
		rc.InflateRect(10, 10);
		if (!rc.PtInRect(pt)) {
			KillTimer(nIDEvent);
			hideWindow();
		}
	} else if (nIDEvent == IDT_AUTO_SWITCH_STATUS)
		onAutoStatus();
	else
		CCaptionBtnDlg::OnTimer(nIDEvent);
}

void CIcqDlg::OnSysMessage() 
{
	IcqMsg *msg = fetchMsg(NULL);
	IcqWindow *win;

	if (!msg) {
		CSysHistoryDlg *dlg;
		win = findWindow(WIN_SYS_HISTORY);
		if (!win)
			dlg = new CSysHistoryDlg;
		else
			dlg = (CSysHistoryDlg *) win;

		dlg->setTop();

	} else {
		CSysMsgDlg *dlg = new CSysMsgDlg(msg);
		dlg->setTop();
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

void CIcqDlg::OnSmallIcon() 
{
	outbarCtrl.setIconView(FALSE);
	saveGroupInfo();
}

void CIcqDlg::OnUpdateSmallIcon(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(outbarCtrl.isLargeIconView());
}

void CIcqDlg::OnLargeIcon() 
{
	outbarCtrl.setIconView(TRUE);
	saveGroupInfo();
}

void CIcqDlg::OnUpdateLargeIcon(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!outbarCtrl.isLargeIconView());
}

void CIcqDlg::OnFontColor() 
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK) {
		outbarCtrl.setFontColor(dlg.GetColor());
		saveGroupInfo();
	}
}

void CIcqDlg::OnBgColor() 
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK) {
		outbarCtrl.setBgColor(dlg.GetColor());
		saveGroupInfo();
	}
}

void CIcqDlg::OnAddGroup() 
{
	outbarCtrl.addFolderUI();
	saveGroupInfo();
}

void CIcqDlg::OnRename()
{
	outbarCtrl.renameUI();
}

void CIcqDlg::OnUpdateAddGroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(outbarCtrl.getSelFolder() < outbarCtrl.getFolderCount() - 2);
}

void CIcqDlg::OnRemove() 
{
	int obj = outbarCtrl.hitObj();
	int index = outbarCtrl.hitIndex();

	if (obj == OutBarCtrl::HT_FOLDER) {
		// Remove a folder
		if (outbarCtrl.getItemCount(index) > 0)
			AfxMessageBox(IDS_GROUP_NOT_EMPTY);
		else {
			outbarCtrl.removeFolder(index);
			saveGroupInfo();
		}
	} else if (obj == OutBarCtrl::HT_ITEM) {
		// Remove an item
		IcqContact *c = outbarCtrl.contact(index);
		if (!c)
			return;
		int n = outbarCtrl.getFolderCount();
		if (outbarCtrl.getSelFolder() < n - 2)
			delFriend(c);
		else {
			outbarCtrl.removeItem(index);
			contactList.remove(c);
			IcqDB::delContact(c->qid);
			saveGroupInfo();
			delete c;
		}
	}
}

void CIcqDlg::OnModifyInfo() 
{
	new CModifyDetailDlg;
}

void CIcqDlg::OnShowOnline() 
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
			getUdpSession()->changeStatus(myInfo.status);
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
			getUdpSession()->changeStatus(myInfo.status);
		}
		destroyTcpSession();
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

/*
 * Called when a plugin menu item is clicked
 */
void CIcqDlg::OnPlugin(UINT nID)
{
	int pos = nID - ID_PLUGIN;
	PtrList &l = PluginFactory::getPluginList();
	PtrList::iterator it;
	IcqPlugin *p = NULL;
	int i = 0;

	for (it = l.begin(); it != l.end(); ++it) {
		p = (IcqPlugin *) *it;
		int type = p->info.type;
		if (type == ICQ_PLUGIN_NET || type == ICQ_PLUGIN_EXE) {
			if (++i > pos)
				break;
		}
	}

	if (it != l.end()) {
		p = (IcqPlugin *) *it;
		new CSendRequestDlg(p->name.c_str(), curContact);
	}
}

/*
 * A reply due to a previous WSAGetHostByName
 */
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

/*
 * The registered global hot key is pressed
 */
LRESULT CIcqDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ID_HOTKEY)
		onNotifyDblClk();

	return 1;
}

/*
 * System tray icon messages
 */
LRESULT CIcqDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	if (wParam != ID_NOTIFYICON || !myInfo.qid.uin)
		return 0;

	// When this message is received, we do not know whether it is the first click of
	// a double-clicked sequence.
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
		if (getUdpSession()->sessionCount) {
			CString str;
			str.Format(IDS_NR_ONLINES, getUdpSession()->sessionCount);
			strcpy(pTTT->szText, str);
		} else {
			pTTT->lpszText = (LPSTR) (LPCTSTR) AfxGetAppName();
			pTTT->hinst = NULL;
		}
	}
	return TRUE;
}

/*
 * Since MFC can not route WM_MEASUREITEM of a popup menu to the specific CMenu,
 * we must do it manually.
 */
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
		DestroyWindow();
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

	CCaptionBtnDlg::OnDestroy();
}

/*
 * To realize the animation effect of a task-bar style window
 */
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
		pWnd->ActivateFrame(&curContact->qid);
}

void CIcqDlg::OnMessageMgr() 
{
	CMsgMgrWnd *pWnd = new CMsgMgrWnd;
	if (pWnd->LoadFrame(IDR_MSGMGR))
		pWnd->ActivateFrame();
}

void CIcqDlg::OnBroadcastMsg() 
{
	new CSendMsgDlg(NULL);
}

void CIcqDlg::OnHomepage() 
{
	ShellExecute(NULL, _T("open"), MYICQ_HOMEPAGE, NULL, NULL, SW_SHOW);
}

void CIcqDlg::OnClose() 
{
	DestroyWindow();
}

void CIcqDlg::PostNcDestroy() 
{
	delete this;
}

void CIcqDlg::OnGroupDetail() 
{
	new CGroupInfoDlg(outbarCtrl.group(currentItem));
}

void CIcqDlg::OnGroupSendMsg() 
{
	IcqGroup *g = outbarCtrl.group(currentItem);
	onGroupDoubleClicked(g);
}

void CIcqDlg::OnGroupSearch()
{
	if (!findWindow(WIN_GROUP_SEARCH_WIZARD))
		new CGroupSearchWizard;
}

void CIcqDlg::OnGroupCreate()
{
	if (!findWindow(WIN_GROUP_CREATE_WIZARD))
		new CGroupCreateWizard;
}

void CIcqDlg::OnGroupExit() 
{
	if (AfxMessageBox(IDS_PROMPT_EXIT_GROUP, MB_YESNO) != IDYES)
		return;

	IcqGroup *g = outbarCtrl.group(currentItem);
	g->destroyAll();
}
