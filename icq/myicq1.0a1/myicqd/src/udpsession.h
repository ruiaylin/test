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

#include "refobject.h"
#include "session.h"
#include "list.h"
#include "slab.h"
#include "icqsocket.h"
#include "dbmanager.h"
#include <time.h>

#define MAX_NICK			16
#define MAX_PROVINCE		16

#define SEND_TIMEOUT		10
#define KEEPALIVE_TIMEOUT	(3 * 60)

enum {
	STATUS_ONLINE,
	STATUS_OFFLINE,
	STATUS_AWAY,
	STATUS_INVIS
};

enum {
	MSG_TEXT,
	MSG_AUTO_REPLY,
	MSG_AUTH_ACCEPTED,
	MSG_AUTH_REQ,
	MSG_AUTH_REJECTED,
	MSG_ADDED,
	MSG_BROADCAST,
};


enum {
	UDP_ACK = 1,
	UDP_NEW_UIN,
	UDP_GET_CONTACTLIST,
	UDP_LOGIN,
	UDP_LOGOUT,
	UDP_KEEPALIVE,
	UDP_CHANGE_STATUS,
	UDP_UPDATE_CONTACT,
	UDP_MODIFY_USER,
	UDP_UPDATE_USER,
	UDP_SEND_MSG,
	UDP_SEARCH_RANDOM,
	UDP_SEARCH_CUSTOM,
	UDP_ADD_FRIEND,
	UDP_DEL_FRIEND,
	UDP_BROADCAST_MSG,
	UDP_GET_SERVER_LIST,
	UDP_GET_GROUP_LIST,
	UDP_SEARCH_GROUP,
	UDP_GET_REMOTE_CONTACTLIST,

	UDP_CREATE_GROUP = 0x80,
	UDP_ENTER_GROUP,
	UDP_EXIT_GROUP,
	UDP_GROUP_START,
	UDP_GROUP_MESSAGE,
	UDP_GROUP_CMD,

	UDP_SRV_USER_ONLINE = 0x0100,
	UDP_SRV_USER_OFFLINE,
	UDP_SRV_MULTI_ONLINE,
	UDP_SRV_STATUS_CHANGED,
	UDP_SRV_MESSAGE,
	UDP_SRV_SEARCH,

	UDP_SRV_GROUP_TYPES = 0x1000,
	UDP_SRV_ENTER_GROUP,
	UDP_SRV_EXIT_GROUP,
};


class UdpOutPacket;
class UdpInPacket;
class Server;
class IcqGroup;

class UdpSession : public RefObject, public Session {
public:
	UdpSession(UdpInPacket &in, uint32 ip, uint16 port);
	~UdpSession();

	UdpOutPacket *createPacket(uint16 cmd, uint16 ackseq = 0);
	void sendPacket(UdpOutPacket *p);

	void sendOnline(Session *s, ICQ_STR &domain);
	void sendOffline(uint32 uin, ICQ_STR &domain);
	void sendStatusChanged(Session *s, ICQ_STR &domain);
	void sendMessage(uint8 type, QID &src, uint32 when, ICQ_STR &text);
	void sendGroupTypes();
	void updateContactReply(uint16 seq, uint8 *data, int n, Server *server = NULL);
	void searchRandomReply(uint16 seq, uint8 *data, int n);
	void searchUINReply(uint16 seq, uint8 *data, int n);

	void dead();
	void logout();

	void onlineNotify();
	void offlineNotify();
	void statusNotify(uint32 newStatus);

	static bool init();
	static void destroy();
	static bool onReceive();
	static void checkSendQueue();
	static void checkKeepAlive();

	static void sendMessage(uint8 type, QID &dst, QID &src, uint32 when, ICQ_STR &text);
	static void addFriend(uint16 seq, QID &dst, uint32 src, Server *server = NULL);
	static void addFriendAuth(uint16 seq, QID &dst, uint32 src, Server *server, uint8 auth, bool sendAuthMsg = true);
	static void updateContact(uint16 seq, uint32 dst, uint32 src, Server *server = NULL);
	static void searchUIN(uint16 seq, uint32 dst, uint32 src, Server *server = NULL);

	uint8 auth;
	uint8 face;
	char nickname[MAX_NICK + 1];
	char province[MAX_PROVINCE + 1];

	uint16 tcpver;
	uint16 port;
	uint32 oldMsgID, lastMsgID;
	char subkey[128];

	ListHead uinItem;
	ListHead ipportItem;
	ListHead listItem;

	static int sock;
	static uint32 sessionCount;

private:
	void createPacket(UdpOutPacket &out, uint16 cmd, uint16 seq, uint16 ackseq);
	void sendDirect(UdpOutPacket *p);
	void sendAckPacket(uint16 seq);
	bool setWindow(uint16 seq);
	void notify(DB_CALLBACK cb1, DB_CALLBACK cb2);

	void onAck(uint16 seq);
	void onKeepAlive(UdpInPacket &in);
	void onNewUIN(UdpInPacket &in);
	void onGetContactList(UdpInPacket &in);
	void onGetRemoteContactList(UdpInPacket &in);
	void onLogin(UdpInPacket &in);
	void onLogout(UdpInPacket &in);
	void onChangeStatus(UdpInPacket &in);
	void onUpdateContact(UdpInPacket &in);
	void onModifyUser(UdpInPacket &in);
	void onUpdateUser(UdpInPacket &in);
	void onSendMessage(UdpInPacket &in);
	void onSearchRandom(UdpInPacket &in);
	void onSearchCustom(UdpInPacket &in);
	void onAddFriend(UdpInPacket &in);
	void onDelFriend(UdpInPacket &in);
	void onSendBCMsg(UdpInPacket &in);
	void onGetServerList(UdpInPacket &in);
	void onGetGroupList(UdpInPacket &in);
	void onSearchGroup(UdpInPacket &in);

	void onCreateGroup(UdpInPacket &in);
	void onEnterGroup(UdpInPacket &in);
	void onExitGroup(UdpInPacket &in);
	void onGroupStart(UdpInPacket &in);
	void onGroupMessage(UdpInPacket &in);
	void onGroupCmd(UdpInPacket &in);

	bool onPacketReceived(UdpInPacket &in);

	uint16 udpver;
	uint32 sid;
	uint16 sendSeq;
	uint16 recvSeq;
	uint32 window;
	time_t expire;

	uint8 isDead : 1;

	IcqGroup *group;

	ListHead sendQueue;

	DECLARE_SLAB(UdpSession)
};


#endif
