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

#ifndef _SERVER_SESSION_H
#define _SERVER_SESSION_H

#include "udpsession.h"
#include "sessionlistener.h"
#include "sockssession.h"
#include "httpsession.h"


class IcqInfo;
class IcqUser;

class ServerSession : public UdpSession, public SessionListener {
public:
	ServerSession(IcqLink *link);
	virtual ~ServerSession();

	virtual void sessionFinished(bool success);
	void connect(const char *host, uint16 port);

	uint16 regNewUIN(const char *passwd);
	uint16 getContactList();
	void sendKeepAlive();
	uint16 login(const char *passwd, uint32 status = 0);
	void logout();
	void changeStatus(uint32 status);
	uint16 updateContact(uint32 uin);
	uint16 modifyUser(IcqUser *info, uint8 modifyPasswd = 0);
	void updateUser();
	uint16 sendMessage(uint8 type, uint32 to, const char *text);
	uint16 groupSendMessage(uint8 type, UinList &uinList, const char *text);
	uint16 searchRandom();
	uint16 searchCustom(uint32 uin, const char *nick, const char *email, uint32 startUIN);
	uint16 addFriend(uint32 uin);
	uint16 delFriend(uint32 uin);
	uint16 broadcastMsg(uint8 type, uint32 expire, const char *text);

	uint16 ackseq;
	uint32 sessionCount;

private:
	virtual bool onPacketReceived(UdpInPacket &in);
	virtual void sendDirect(UdpOutPacket *p);
	virtual void onSendError(UdpOutPacket *p);

	uint16 sendPacket(UdpOutPacket *);
	void getInfo(IcqInfo &info, UdpInPacket &);

	void onNewUINReply(UdpInPacket &);
	void onContactListReply(UdpInPacket &);
	void onLoginReply(UdpInPacket &);
	void onUserOnline(UdpInPacket &);
	void onUserOffline(UdpInPacket &);
	void onMultiOnline(UdpInPacket &);
	void onStatusChanged(UdpInPacket &);
	void onUpdateContactReply(UdpInPacket &);
	void onUpdateUserReply(UdpInPacket &);
	void onRecvMessage(UdpInPacket &);
	void onSearchReply(UdpInPacket &);
	void onAddFriendReply(UdpInPacket &);

	uint32 realIP;
	SocksSession socksSession;
	HttpSession httpSession;
};


#endif