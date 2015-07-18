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

// myicqDlg.h : header file
//

#if !defined(AFX_MYICQDLG_H__A1176E03_E6E7_4E6C_93BB_503F26D81C97__INCLUDED_)
#define AFX_MYICQDLG_H__A1176E03_E6E7_4E6C_93BB_503F26D81C97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "icqlink.h"
#include "CaptionBtnDlg.h"
#include "MyICQCtrl.h"
#include "BtnST.h"
#include "icqskin.h"

/////////////////////////////////////////////////////////////////////////////
// CIcqDlg dialog

class CIcqDlg : public CCaptionBtnDlg, public IcqLink, public OutBarListener
{
// Construction
public:
	CIcqDlg(CWnd* pParent = NULL);	// standard constructor
	~CIcqDlg();

	void onSendError(uint32 seq);
	void onConnect(bool success);

	void onNewUINReply(uint32 uin);
	void onContactListReply(UinList &l);
	void onLoginReply(uint16 error);
	void onUserOnline(uint32 uin, uint32 status, uint32 ip, uint16 port, uint32 realIP, bool multi);
	void onUserOffline(uint32 uin);
	void onStatusChanged(uint32 uin, uint32 status);
	void onUpdateContactReply(IcqContact *info);
	void onUpdateUserReply();
	void onRecvMessage(IcqMsg *msg);
	void onUserFound(PtrList *l);
	void onAddFriendReply(uint32 uin, uint8 result);

	virtual void *getFaceIcon(int face, int status);
	virtual void popupMsg(void *icon, const char *text, uint32 t);

	virtual void renamed(int obj, int index, const char *text);
	virtual void itemDragged(int item, int toFolder);
	virtual void selFolderChanged(int folder);
	virtual void itemClicked(int item);
	virtual void itemDoubleClicked(int item);
	virtual void rightButtonDown(int obj, int index);

	HANDLE getHostByName(const char *name);
	IcqMsg *fetchMsg(uint32 uin);
	void showOptions(UINT iSelectPage = 0);

	MyICQCtrl outbarCtrl;

private:
	virtual void drawCaptionBtn(CDC *pDC, CRect &rc, int button, BOOL pressed);
	virtual void onCaptionBtnClicked(int button);

	void onError();
	void onStatusAway();
	void onNotifyDblClk();
	void onItemDoubleClicked(IcqContact *c);
	void destroyUser();
	void resolveHost();
	void onHostFound(in_addr &addr);
	void onlineNotify(IcqContact *c);
	void addNotifyIcon();
	void removeNotifyIcon();
	void modifyNotifyIcon(HICON hIcon);
	void popupMainMenu(UINT nFlags, CPoint &pt);
	void popupStatusMenu(UINT nFlags, CPoint &pt);
	void updateStatusIcon();
	void animateStatus();
	void hideWindow(BOOL hide = TRUE);
	void addPendingMsg(IcqMsg *msg);

	void login(uint32 status = 0);
	void logout();
	void addContact(IcqContact *c, int folder = 0);
	IcqContact *addContact(uint32 uin, int folder = 0);
	BOOL doRegWizard();
	BOOL doLogin();
	void setDefaultOptions();
	void addFriend(IcqContact *c, int folder = -1);
	BOOL delFriend(IcqContact *c, int folder = -1);
	void saveGroupInfo();
	BOOL loadGroupInfo();
	void initGUI();
	BOOL loadPlugins();

	HICON iconSysMsg;
	HICON iconBlank;
	CToolTipCtrl toolTip;
	CImageList statusImgList;
	IcqContact *curContact;
	uint32 pendingStatus;
	uint16 lastSeq;
	char hostBuf[MAXGETHOSTSTRUCT];
	UinList uinList;
	int regMode;
	int alignType;
	CSize winNormalSize;
	BOOL isHide;
	IcqSkin skin;
	CMenu *currentMenu;

// Dialog Data
	//{{AFX_DATA(CIcqDlg)
	enum { IDD = IDD_MYICQ_DIALOG };
	CButtonST	m_btnStat;
	CButtonST	m_btnAbout;
	CButtonST	m_btnMain;
	CButtonST	m_btnStatus;
	CButtonST	m_btnSysMsg;
	CButtonST	m_btnSearch;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcqDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIcqDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSendMessage();
	afx_msg void OnViewDetail();
	afx_msg void OnFindContact();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSysMessage();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnMainMenu();
	afx_msg void OnGfxSmallIcon();
	afx_msg void OnUpdateGfxSmallIcon(CCmdUI* pCmdUI);
	afx_msg void OnGfxLargeIcon();
	afx_msg void OnUpdateGfxLargeIcon(CCmdUI* pCmdUI);
	afx_msg void OnGfxFontColor();
	afx_msg void OnGfxBackground();
	afx_msg void OnGfxAddGroup();
	afx_msg void OnUpdateGfxAddGroup(CCmdUI* pCmdUI);
	afx_msg void OnGfxRemove();
	afx_msg void OnGfxRename();
	afx_msg void OnModifyInfo();
	afx_msg void OnGfxShowOnline();
	afx_msg void OnStatus();
	afx_msg void OnStatusOnline();
	afx_msg void OnStatusOffline();
	afx_msg void OnStatusInvis();
	afx_msg void OnSysOption();
	afx_msg void OnStatusAwayCustom();
	afx_msg void OnStatusAwayNoReply();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnRegWizard();
	afx_msg void OnChangeUser();
	afx_msg void OnAbout();
	afx_msg void OnDestroy();
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSendEmail();
	afx_msg void OnHomePage();
	afx_msg void OnAddAsFriend();
	afx_msg void OnMsgHistory();
	afx_msg void OnMessageMgr();
	afx_msg void OnBroadcastMsg();
	//}}AFX_MSG
	afx_msg LRESULT OnOutBarNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHostFound(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStatusAway(UINT nID);
	afx_msg void OnPlugin(UINT nID);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYICQDLG_H__A1176E03_E6E7_4E6C_93BB_503F26D81C97__INCLUDED_)
