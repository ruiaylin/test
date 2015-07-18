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

#ifndef _ICQ_LINK_H
#define _ICQ_LINK_H

#include "icqclient.h"
#include "icqlinkbase.h"

class IcqSession;
class IcqWindow;
class IcqPlugin;
class TcpSession;

class IcqLink : public IcqLinkBase {
public:
	IcqLink();
	virtual ~IcqLink();

	bool isProxyType(int type) {
		return (options.flags.test(UF_USE_PROXY) && (options.proxyType == type));
	}
	void checkSendQueue();

	IcqSession *createSession(const char *name, uint32 uin);
	TcpSession *acceptSession(int listenSock, const char *name);
	uint32 sendMessage(uint8 type, uint32 to, const char *text);
	void onRecvMessage(uint8 type, uint32 from, uint32 when, const char *text, bool relay);
	uint32 sendTcpRequest(const char *name, IcqContact *c, const char *text);
	void acceptTcpRequest(const char *name, IcqContact *c, uint16 port);

	virtual void onNewUINReply(uint32 uin) = 0;
	virtual void onContactListReply(UinList &l) = 0;
	virtual void onLoginReply(uint16 error) = 0;
	virtual void onUpdateContactReply(IcqContact *info);
	virtual void onUpdateUserReply() = 0;
	virtual void onUserOnline(uint32 uin, uint32 status, uint32 ip, uint16 port, uint32 realIP, bool multi = false) = 0;
	virtual void onUserOffline(uint32 uin);
	virtual void onStatusChanged(uint32 uin, uint32 status) = 0;
	virtual void onUserFound(PtrList *l) = 0;
	virtual void onAddFriendReply(uint32 uin, uint8 result) = 0;
	virtual void onRecvMessage(IcqMsg *msg) = 0;
	virtual void onConnect(bool success) = 0;

	virtual ContactInfo *getContactInfo(uint32 uin);
	virtual TcpSessionBase *createTcpSession(TcpSessionListener *l, uint32 ip, uint16 port);
	virtual void destroySession(const char *name, uint32 uin);
	virtual void onSendError(uint32 seq);

	void onAck(uint32 seq);
	void addPendingMsg(IcqMsg *msg);

	IcqContact *findContact(uint32 uin, int type = -1);
	IcqWindow *findWindow(int type, uint32 uin = 0, uint32 seq = 0);
	IcqWindow *findWindowSeq(uint32 seq);
	IcqSession *findSession(const char *name, uint32 uin);
	IcqMsg *findPendingMsg(uint32 uin);

	uint32 socksIP;
	IcqUser myInfo;
	IcqOption options;
	
	PtrList sessionList;
	PtrList windowList;
	PtrList msgList;

protected:
	void logout();
	void destroyUser();
	uint16 createListenSession(const char *name);
	void destroySession(uint32 uin);
	void destroySession();
	void destroyContact(uint32 uin);

	PtrList contactList;
};


extern IcqLink *icqLink;


class ServerSession;

inline ServerSession *serverSession()
{
	return (ServerSession *) icqLink->sessionList.front();
}

#endif
