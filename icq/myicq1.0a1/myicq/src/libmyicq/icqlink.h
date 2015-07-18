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
#include "icqprofile.h"

class UdpSession;
class TcpSession;
class IcqWindow;
class GroupWindow;
class IcqGroup;
class GroupPlugin;
class InPacket;

class IcqLink : public IcqLinkBase {
public:
	IcqLink();
	virtual ~IcqLink();

	bool isProxyType(int type) {
		return (options.flags.test(UF_USE_PROXY) && (options.proxyType == type));
	}

	uint16 createListenSession(const char *name);
	TcpSession *createTcpSession(const char *name, QID &qid);
	void removeTcpSession(TcpSession *s);
	TcpSession *acceptSession(int listenSock, const char *name);
	IcqGroup *createGroup(const char *plugin, uint32 id);
	uint32 sendMessage(uint8 type, QID &to, const char *text, bool relay = false);
	uint32 sendMessage(IcqMsg &msg);
	void cancelSendMessage(QID &qid);
	void onRecvMessage(uint8 type, QID &from, uint32 when, const char *text, bool relay);
	uint32 sendTcpRequest(const char *name, IcqContact *c, const char *text);
	void acceptTcpRequest(const char *name, IcqContact *c, uint16 port);

	virtual void onNewUINReply(QID &qid) = 0;
	virtual void onContactListReply(QIDList &l) = 0;
	virtual void onRemoteContactList(const char *domain, QIDList &l) = 0;
	virtual void onLoginReply(uint16 error, uint32 ip) = 0;
	virtual void onUpdateContactReply(IcqContact *info);
	virtual void onUpdateUserReply() = 0;
	virtual void onUserOnline(QID &qid, uint32 status, uint32 ip, uint16 port, uint32 realIP, bool multi = false) = 0;
	virtual void onUserOffline(QID &qid);
	virtual void onStatusChanged(QID &qid, uint32 status) = 0;
	virtual void onUserFound(PtrList *l) = 0;
	virtual void onAddFriendReply(QID &qid, uint8 result) = 0;
	virtual void onRecvMessage(IcqMsg *msg) = 0;
	virtual void onConnect(bool success) = 0;
	virtual void onServerListReply(SERVER_INFO info[], int n) = 0;
	virtual void onGroupTypes(PtrArray &a);
	virtual void onGroupListReply(PtrList &l) = 0;
	virtual void onSearchGroupReply(GROUP_INFO *info) = 0;
	virtual void onCreateGroupReply(uint32 id) = 0;
	virtual void onEnterGroupReply(uint32 id, uint16 error) = 0;
	virtual void onMemberEntered(uint32 id, uint32 uin, uint8 face, const char *nick);
	virtual void onMemberExited(uint32 id, uint32 uin);
	virtual void onGroupStart(uint32 id);
	virtual void onGroupCmd(uint32 id, InPacket &in);
	virtual void onGroupMessage(uint32 id, uint32 from, uint32 when, const char *text);
	virtual void onGroupMessage(IcqMsg *msg) = 0;

	virtual ContactInfo *getContactInfo(QID *qid);
	virtual TcpSessionBase *createTcpSession(TcpSessionListener *l, uint32 ip, uint16 port);
	virtual void onSendError(uint32 seq);
	virtual void exitGroup(IcqGroup *g);

	void onAck(uint32 seq);

	void addPendingMsg(IcqMsg *msg);
	void changeStatus(int status);

	IcqContact *findContact(QID &qid, int type = -1);
	IcqGroup *findGroup(uint32 id);
	IcqWindow *findWindow(int type, QID *qid = NULL, uint32 seq = 0);
	IcqWindow *findWindowSeq(uint32 seq);
	GroupWindow *findGroupWindow(uint32 id);
	GroupWindow *findGroupWindowSeq(uint32 seq);
	TcpSession *findTcpSession(const char *name, QID &qid);
	IcqMsg *findPendingMsg(QID *qid);
	IcqMsg *findPendingMsg(uint32 id);
	GROUP_TYPE_INFO *getGroupTypeInfo(int type);

	uint32 ourIP;
	IcqUser myInfo;
	IcqOption options;
	IcqProfile myProfile;

	PtrList tcpSessionList;
	PtrList windowList, groupWindowList;
	PtrList msgList;
	PtrArray groupTypes;

	UdpSession *udpSession;

protected:
	void logout();
	void initUser(const char *dir);
	void destroyUser();
	bool registerExePlugin(const char *path, const char *name);
	void destroyTcpSession(QID &qid);
	void destroyTcpSession();
	void destroyContact(QID &qid);

	PtrList contactList;
	PtrList groupList;
};


extern IcqLink *icqLink;


class ServerSession;

inline UdpSession *getUdpSession()
{
	return icqLink->udpSession;
}

#endif
