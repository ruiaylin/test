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

#ifndef _UDP_SESSION_H
#define _UDP_SESSION_H

#include "icqtypes.h"
#include "icqsocket.h"
#include "proxy.h"
#include "socksproxy.h"
#include "httpproxy.h"

#define SEND_TIMEOUT			10
#define KEEPALIVE_TIMEOUT		(2 * 60)

enum {
	LOGIN_SUCCESS,
	LOGIN_INVALID_UIN,
	LOGIN_WRONG_PASSWD,
};

enum {
	GROUP_ERROR_SUCCESS,
	GROUP_ERROR_NOT_EXIST,
	GROUP_ERROR_ALREADY_EXIST,
	GROUP_ERROR_EXCEED_MAX_GROUPS,
	GROUP_ERROR_WRONG_PASSWD,
	GROUP_ERROR_EXCEED_MAX_MEMBERS,
};

enum {
	ADD_FRIEND_ACCEPTED,
	ADD_FRIEND_AUTH_REQ,
	ADD_FRIEND_REJECTED,
};


class IcqLink;
class IcqInfo;
class IcqUser;
class UdpOutPacket;
class UdpInPacket;

class UdpSession : public SocketListener, public ProxyListener {
public:
	UdpSession(IcqLink *link);
	virtual ~UdpSession();

	virtual bool onReceive();
	virtual void onEstablished(bool success);

	UdpOutPacket *createGroupPacket(uint32 id);
	uint16 sendPacket(UdpOutPacket *out);

	void connect(const char *host, uint16 port);
	void checkSendQueue();

	uint16 regNewUIN(const char *passwd);
	uint16 getContactList();
	uint16 getRemoteContactList();
	void sendKeepAlive();
	uint16 login(const char *passwd, uint32 status);
	void logout();
	void changeStatus(uint32 status);
	uint16 updateContact(QID &qid);
	uint16 modifyUser(IcqUser *info, uint8 modifyPasswd = 0);
	void updateUser();
	uint16 sendMessage(uint8 type, QID &qid, const char *text);
	uint16 searchRandom(const char *domain);
	uint16 searchUIN(QID &qid);
	uint16 searchCustom(const char *nick, const char *email, uint32 startUIN);
	uint16 addFriend(QID &qid);
	uint16 delFriend(QID &qid);
	uint16 sendBCMsg(uint8 type, const char *text);
	uint16 getServerList();
	uint16 getGroupList(uint16 type);
	uint16 searchGroup(uint32 id);
	uint16 createGroup(uint16 type, const char *name, const char *pass);
	uint16 enterGroup(uint32 id, const char *pass);
	void exitGroup(uint32 id);
	uint16 sendGroupMessage(uint32 id, const char *text);
	void startGroup(uint32 id);

	// Only for convenient access
	uint16 ackseq;

	// The number of currently online users
	uint32 sessionCount;

private:
	virtual bool onPacketReceived(UdpInPacket &in);
	virtual void sendDirect(UdpOutPacket *p);
	virtual void onSendError(UdpOutPacket *p);

	void createPacket(UdpOutPacket &out, uint16 cmd, uint16 seq);
	UdpOutPacket *createPacket(uint16 cmd);
	void sendAckPacket(uint16 seq);
	void sendDirect(UdpOutPacket *out, int s);

	void initSession();
	void clearSendQueue();
	bool setWindow(uint16 seq);

	void getInfo(IcqInfo &info, UdpInPacket &);

	bool onAck(uint16 seq);
	void onNewUINReply(UdpInPacket &in);
	void onContactListReply(UdpInPacket &in);
	void onRemoteContactList(UdpInPacket &in);
	void onLoginReply(UdpInPacket &in);
	void onUserOnline(UdpInPacket &in);
	void onUserOffline(UdpInPacket &in);
	void onMultiOnline(UdpInPacket &in);
	void onStatusChanged(UdpInPacket &in);
	void onUpdateContactReply(UdpInPacket &in);
	void onUpdateUserReply(UdpInPacket &in);
	void onRecvMessage(UdpInPacket &in);
	void onSearchReply(UdpInPacket &in);
	void onAddFriendReply(UdpInPacket &in);
	void onServerListReply(UdpInPacket &in);
	void onGroupTypes(UdpInPacket &in);
	void onGroupListReply(UdpInPacket &in);
	void onSearchGroupReply(UdpInPacket &in);

	void onCreateGroupReply(UdpInPacket &in);
	void onEnterGroupReply(UdpInPacket &in);
	void onMemberEntered(UdpInPacket &in);
	void onMemberExited(UdpInPacket &in);
	void onGroupStart(UdpInPacket &in);
	void onGroupMessage(UdpInPacket &in);
	void onGroupCmd(UdpInPacket &in);

	IcqLink *icqLink;

	int sock;
	uint32 sid;			// Session ID
	uint16 sendSeq;
	uint8 window[1 << 13];
	PtrList sendQueue;
	int retryKeepAlive;

	uint32 destIP;
	uint32 realIP;

	// proxy sessions
	SocksProxy socksProxy;
	HttpProxy httpProxy;
};

#endif
