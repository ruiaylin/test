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

#include <iostream>
#include "serversession.h"
#include "icqlink.h"
#include "udppacket.h"
#include "ndes.h"

#define MAX_SEARCH_PER_PAGE		25


ServerSession::ServerSession(IcqLink *link)
: UdpSession(link, ICQ_SESSION_SERVER, 0)
{	
	ackseq = 0;
	sessionCount = 0;
	realIP = 0;

	memset(&proxyAddr, 0, sizeof(proxyAddr));

	udpSock = IcqSocket::createSocket(SOCK_DGRAM, this);

	socksSession.setListener(this);
	httpSession.setListener(this);
}

ServerSession::~ServerSession()
{
	if (udpSock >= 0)
		IcqSocket::closeSocket(udpSock);
}

void ServerSession::sendDirect(UdpOutPacket *p)
{
	if (icqLink->isProxyType(PROXY_HTTP)) {
		uint16 n = p->getSize() - sizeof(uint16);
		int old = p->setCursor(0);
		*p << n;
		p->setCursor(old);
		UdpSession::sendDirect(p, httpSession.sock);
	} else
		UdpSession::sendDirect(p);
}

void ServerSession::onSendError(UdpOutPacket *p)
{
	if (p->cmd == UDP_KEEPALIVE)
		icqLink->onConnect(false);
	else
		UdpSession::onSendError(p);
}

void ServerSession::connect(const char *host, uint16 port)
{
	destHost = host;
	UdpSession::connect(ntohl(inet_addr(host)), port);

	IcqOption &options = icqLink->options;

	if (options.flags.test(UF_USE_PROXY)) {
		if (options.proxyType == PROXY_SOCKS) {
			sockaddr_in addr;
			socklen_t addrlen = sizeof(addr);
			getsockname(udpSock, (sockaddr *) &addr, &addrlen);

			socksSession.start(options.proxy[PROXY_SOCKS], addr.sin_port);

		} else if (options.proxyType == PROXY_HTTP)
			httpSession.start(host, options.proxy[PROXY_HTTP]);
	} else
		icqLink->onConnect(true);
}

void ServerSession::sendKeepAlive()
{
	UdpOutPacket *out = createPacket(UDP_KEEPALIVE);
	sendPacket(out);
}

uint16 ServerSession::sendPacket(UdpOutPacket *p)
{
	if (p->cmd != UDP_NEW_UIN && p->cmd != UDP_LOGIN)
		p->encrypt();
	return UdpSession::sendPacket(p);
}

void ServerSession::getInfo(IcqInfo &info, UdpInPacket &in)
{
	in >> info.face >> info.nick >> info.age >> info.gender;
	in >> info.country >> info.province >> info.city;

	in >> info.email >> info.address >> info.zipcode >> info.tel;
	
	in >> info.name >> info.blood >> info.college;
	in >> info.profession >> info.homepage >> info.intro;
}

uint16 ServerSession::regNewUIN(const char *passwd)
{
	initSession();
	uin = 0;
	
	UdpOutPacket *out = createPacket(UDP_NEW_UIN);
	*out << passwd;
	return sendPacket(out);
}

uint16 ServerSession::getContactList()
{
	UdpOutPacket *out = createPacket(UDP_GET_CONTACTLIST);
	return sendPacket(out);
}

uint16 ServerSession::login(const char *passwd, uint32 status)
{
	initSession();

	char pass[8];
	strncpy(pass, passwd, sizeof(pass));
	setkey(pass);

	UdpOutPacket *out = createPacket(UDP_LOGIN);
	*out << passwd << status << (uint16) MYICQ_TCP_VER << realIP;
	return sendPacket(out);
}

void ServerSession::logout()
{
	UdpOutPacket *out = createPacket(UDP_LOGOUT);
	sendPacket(out);
	clearSendQueue();
}

void ServerSession::changeStatus(uint32 status)
{
	UdpOutPacket *out = createPacket(UDP_CHANGE_STATUS);
	*out << status;
	sendPacket(out);
}

uint16 ServerSession::updateContact(uint32 uin)
{
	UdpOutPacket *out = createPacket(UDP_UPDATE_CONTACT);
	*out << uin;
	return sendPacket(out);
}

void ServerSession::updateUser()
{
	UdpOutPacket *out = createPacket(UDP_UPDATE_USER);
	sendPacket(out);
}

uint16 ServerSession::modifyUser(IcqUser *info, uint8 modifyPasswd)
{
	UdpOutPacket *out = createPacket(UDP_MODIFY_USER);

	*out << info->face << info->nick << info->age << info->gender;
	*out << info->country << info->province << info->city;

	*out << info->email << info->address << info->zipcode << info->tel;
	
    *out << info->name << info->blood << info->college;
    *out << info->profession << info->homepage << info->intro;

	*out << info->auth << modifyPasswd;
	if (modifyPasswd)
		*out << info->passwd;

	return sendPacket(out);
}

uint16 ServerSession::sendMessage(uint8 type, uint32 to, const char *text)
{
	time_t when = time(NULL);
	UdpOutPacket *out = createPacket(UDP_SEND_MSG);
	*out << type << to << (uint32) when << text;
	return sendPacket(out);
}

uint16 ServerSession::groupSendMessage(uint8 type, UinList &uinList, const char *text)
{
	time_t when = time(NULL);
	UdpOutPacket *out = createPacket(UDP_GROUP_SEND_MSG);
	*out << type << (uint32) when << text;

	*out << (uint16) uinList.size();
	UinList::iterator it;
	for (it = uinList.begin(); it != uinList.end(); ++it)
		*out << *it;

	return sendPacket(out);
}

uint16 ServerSession::broadcastMsg(uint8 type, uint32 expire, const char *text)
{
	time_t when = time(NULL);
	UdpOutPacket *out = createPacket(UDP_BROADCAST_MSG);
	*out << type << (uint32) when << expire << text;

	return sendPacket(out);
}

uint16 ServerSession::searchRandom()
{
	UdpOutPacket *out = createPacket(UDP_SEARCH_RANDOM);
	return sendPacket(out);
}

uint16 ServerSession::searchCustom(uint32 uin, const char *nick, const char *email, uint32 startUIN)
{
	UdpOutPacket *out = createPacket(UDP_SEARCH_CUSTOM);
	*out << uin << nick << email << startUIN;
	return sendPacket(out);
}

uint16 ServerSession::addFriend(uint32 uin)
{
	UdpOutPacket *out = createPacket(UDP_ADD_FRIEND);
	*out << uin;
	return sendPacket(out);
}

uint16 ServerSession::delFriend(uint32 uin)
{
	UdpOutPacket *out = createPacket(UDP_DEL_FRIEND);
	*out << uin;
	return sendPacket(out);
}

bool ServerSession::onPacketReceived(UdpInPacket &in)
{
	if (!UdpSession::onPacketReceived(in))
		return false;

	uint16 seq = in.getSeq();
	ackseq = in.getAckSeq();
	if (ackseq && !seq)
		icqLink->onAck(ackseq);

	uint16 cmd = in.getCmd();

	switch (cmd) {
	case UDP_NEW_UIN:
		onNewUINReply(in);
		break;

	case UDP_GET_CONTACTLIST:
		onContactListReply(in);
		break;

	case UDP_LOGIN:
		onLoginReply(in);
		break;

	case UDP_KEEPALIVE:
		in >> sessionCount;
		return true;

	case UDP_SRV_MULTI_ONLINE:
		onMultiOnline(in);
		break;

	case UDP_SRV_USER_ONLINE:
		onUserOnline(in);
		break;

	case UDP_SRV_USER_OFFLINE:
		onUserOffline(in);
		break;

	case UDP_SRV_STATUS_CHANGED:
		onStatusChanged(in);
		break;

	case UDP_UPDATE_CONTACT:
		onUpdateContactReply(in);
		break;

	case UDP_UPDATE_USER:
		onUpdateUserReply(in);
		break;

	case UDP_SRV_MESSAGE:
		onRecvMessage(in);
		break;

	case UDP_SRV_SEARCH:
		onSearchReply(in);
		break;

	case UDP_ADD_FRIEND:
		onAddFriendReply(in);
		break;

	default:
		cerr << "unknown cmd " << cmd << endl;
		return false;
	}

	sendAckPacket(in.getSeq());
	return true;
}

void ServerSession::onLoginReply(UdpInPacket &in)
{
	uint8 error;
	in >> error;
	icqLink->onLoginReply(error);
}

void ServerSession::onMultiOnline(UdpInPacket &in)
{
	uint16 num;
	in >> num;
	int n = num;

	while (--n >= 0) {
		uint32 uin, status, ip, realIP;
		uint16 port;
		in >> uin >> status >> ip >> port >> realIP;
		icqLink->onUserOnline(uin, status, ip, port, realIP, true);
	}
}

void ServerSession::onUserOnline(UdpInPacket &in)
{
	uint32 uin, status, ip, realIP;
	uint16 port;
	in >> uin >> status >> ip >> port >> realIP;
	icqLink->onUserOnline(uin, status, ip, port, realIP);
}

void ServerSession::onUserOffline(UdpInPacket &in)
{
	uint32 uin;
	in >> uin;
	icqLink->onUserOffline(uin);
}

void ServerSession::onStatusChanged(UdpInPacket &in)
{
	uint32 uin, status;
	in >> uin >> status;
	icqLink->onStatusChanged(uin, status);
}

void ServerSession::onNewUINReply(UdpInPacket &in)
{
	in >> uin;
	icqLink->onNewUINReply(uin);
}

void ServerSession::onContactListReply(UdpInPacket &in)
{
	uint16 num;
	in >> num;
	
	UinList l;
	while (num-- > 0) {
		uint32 uin;
		in >> uin;
		l.push_back(uin);
	}
	icqLink->onContactListReply(l);
}

void ServerSession::onUpdateContactReply(UdpInPacket &in)
{
	IcqContact info;

	in >> info.uin;
	getInfo(info, in);

	icqLink->onUpdateContactReply(&info);
}

void ServerSession::onUpdateUserReply(UdpInPacket &in)
{
	IcqUser &info = icqLink->myInfo;
	getInfo(info, in);
	in >> info.auth;

	icqLink->onUpdateUserReply();
}

void ServerSession::onRecvMessage(UdpInPacket &in)
{
	uint8 type;
	uint32 from, when;
	const char *text;
	
	in >> type >> from >> when >> text;

	cout << "received message from " << from << endl;
	icqLink->onRecvMessage(type, from, when, text, true);
}

void ServerSession::onSearchReply(UdpInPacket &in)
{
	PtrList *l = new PtrList;
	uint16 num;
	in >> num;
	int n = num;

	while (n-- > 0) {
		SEARCH_RESULT *p = new SEARCH_RESULT;
		in >> p->uin >> p->online >> p->face >> p->nick >> p->province;
		l->push_back(p);
	}
	icqLink->onUserFound(l);
}

void ServerSession::onAddFriendReply(UdpInPacket &in)
{
	uint32 uin;
	uint8 result;
	in >> uin >> result;
	icqLink->onAddFriendReply(uin, result);
}

void ServerSession::sessionFinished(bool success)
{
	if (success && icqLink->options.proxyType == PROXY_SOCKS) {
		sockaddr_in addr;
		socklen_t addrlen = sizeof(addr);
		getsockname(socksSession.sock, (sockaddr *) &addr, &addrlen);
		realIP = ntohl(addr.sin_addr.s_addr);

		memset(&proxyAddr, 0, sizeof(proxyAddr));
		proxyAddr.sin_family = AF_INET;
		proxyAddr.sin_addr.s_addr = socksSession.socksIP;
		proxyAddr.sin_port = socksSession.socksPort;

		icqLink->socksIP = ntohl(socksSession.socksIP);
	}

	icqLink->onConnect(success);
}
