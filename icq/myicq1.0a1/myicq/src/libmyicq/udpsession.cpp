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

#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "udpsession.h"
#include "udppacket.h"
#include "icqlink.h"
#include "msgsession.h"

// Protocol commands
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

#define MYICQ_UDP_VER			1
#define MAX_SEND_ATTEMPTS		2
#define MAX_RETRY_KEEPALIVE		2


UdpSession::UdpSession(IcqLink *link)
{
	icqLink = link;
	
	// This is the main UDP socket
	sock = IcqSocket::createSocket(SOCK_DGRAM, this);

	socksProxy.setListener(this);
	httpProxy.setListener(this);

	destIP = realIP = 0;

	initSession();
}

UdpSession::~UdpSession()
{
	clearSendQueue();
}

void UdpSession::connect(const char *host, uint16 port)
{
	IcqOption &options = icqLink->options;

	// Connect using proxy
	if (options.flags.test(UF_USE_PROXY)) {
		if (options.proxyType == PROXY_SOCKS)
			socksProxy.start(options.proxy[PROXY_SOCKS], sock);
		else if (options.proxyType == PROXY_HTTP)
			httpProxy.start(host, options.proxy[PROXY_HTTP]);

	} else {
		sockaddr_in addr;
		socklen_t len = sizeof(addr);

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(host);
		::connect(sock, (sockaddr *) &addr, len);

		getsockname(sock, (sockaddr *) &addr, &len);
		realIP = ntohl(addr.sin_addr.s_addr);

		icqLink->onConnect(true);
	}
}

/*
 * Resend timed-out packets in the send queue
 */
void UdpSession::checkSendQueue()
{
	time_t now = time(NULL);

	while (!sendQueue.empty()) {
		UdpOutPacket *p = (UdpOutPacket *) sendQueue.front();
		if (p->expire > now)
			break;

		sendQueue.pop_front();
		p->attempts++;
		if (p->attempts <= MAX_SEND_ATTEMPTS) {
			// Resend it
			cout << "retransmit packet " << p->seq << endl;

			p->expire = now + SEND_TIMEOUT;
			sendDirect(p);
			sendQueue.push_back(p);
		}
		else {
			// Maximum attempts reached, delete it
			cout << "packet " << p->seq << " timed out" << endl;
			
			onSendError(p);
			delete p;
		}
	}
}

void UdpSession::clearSendQueue()
{
	PtrList::iterator i;
	for (i = sendQueue.begin(); i != sendQueue.end(); i++)
		delete (UdpOutPacket *) *i;
	sendQueue.clear();
}

/*
 * Initialize a udp session
 */
void UdpSession::initSession()
{
	sid = (rand() & 0x7fffffff) + 1;
	sendSeq = (rand() & 0x3fff);
	retryKeepAlive = 0;
	sessionCount = 0;

	memset(window, 0, sizeof(window));

	clearSendQueue();
}

void UdpSession::createPacket(UdpOutPacket &out, uint16 cmd, uint16 seq)
{
	out << (uint16) MYICQ_UDP_VER << (uint32) 0;
	out << icqLink->myInfo.qid.uin << sid << cmd << seq;
	out << (uint16) 0;		// Checkcode will be calculated later
}

UdpOutPacket *UdpSession::createPacket(uint16 cmd)
{
	UdpOutPacket *p = new UdpOutPacket;
	p->cmd = cmd;
	p->seq = ++sendSeq;
	createPacket(*p, cmd, sendSeq);
	return p;
}

UdpOutPacket *UdpSession::createGroupPacket(uint32 id)
{
	UdpOutPacket *out = createPacket(UDP_GROUP_CMD);
	*out << id;
	return out;
}

void UdpSession::sendAckPacket(uint16 seq)
{
	UdpOutPacket out;
	createPacket(out, UDP_ACK, seq);
	sendDirect(&out);
}

void UdpSession::sendDirect(UdpOutPacket *out, int s)
{
	char buf[MAX_PACKET_SIZE + 256];
	char *p = buf;
	int n = out->getSize();

	IcqOption &options = icqLink->options;

	if (options.flags.test(UF_USE_PROXY)) {
		switch (options.proxyType) {
		case PROXY_HTTP:
			*(uint16 *) p = htons(n);
			p += sizeof(uint16);
			break;

		case PROXY_SOCKS:
			*(uint16 *) p = 0;
			p += sizeof(uint16);
			*p++ = 0;
			if (options.proxy[PROXY_SOCKS].resolve) {
				// IPv4
				*p++ = 1;
				*(uint32 *) p = destIP;
				p += sizeof(uint32);
			} else {
				// Domain name
				*p++ = 3;
				string &domain = icqLink->myInfo.qid.domain;
				uint8 len = domain.length();
				*p++ = len;
				memcpy(p, domain.c_str(), len);
				p += len;
			}
			*(uint16 *) p = htons(options.port);
			p += sizeof(uint16);
			break;
		}
	}
	memcpy(p, out->getData(), n);
	p += n;
	send(s, buf, p - buf, 0);
}

void UdpSession::sendDirect(UdpOutPacket *out)
{
	if (icqLink->isProxyType(PROXY_HTTP))
		sendDirect(out, httpProxy.sock);
	else
		sendDirect(out, sock);
}

void UdpSession::onSendError(UdpOutPacket *p)
{
	if (p->cmd == UDP_KEEPALIVE) {
		// This probably means we have lost connection with the server,
		// report it as a serious problem
		if (++retryKeepAlive >= MAX_RETRY_KEEPALIVE)
			icqLink->onConnect(false);

	} else if (p->cmd == UDP_LOGIN)
		icqLink->onConnect(false);
	else
		icqLink->onSendError(p->seq);
}

/*
 * To see whether this is a duplicated packet
 */
bool UdpSession::setWindow(uint16 seq)
{
	uint8 &byte = window[seq / 8];
	uint8 mask = (1 << (seq % 8));
	if (byte & mask)
		return false;

	byte |= mask;
	return true;
}

/*
 * Called when an acknowleged packet is received
 */
bool UdpSession::onAck(uint16 seq)
{
	PtrList::iterator i;
	for (i = sendQueue.begin(); i != sendQueue.end(); i++) {
		UdpOutPacket *p = (UdpOutPacket *) *i;
		if (p->seq == seq) {
			cout << "packet " << seq << " is ACKed" << endl;
			sendQueue.erase(i);
			delete p;
			return true;
		}
	}

	cout << "ACK packet " << seq << " is ignored" << endl;
	return false;
}

/*
 * Send 'keep alive' message every few minutes to the server to
 * let it know that we are still alive
 */
void UdpSession::sendKeepAlive()
{
	UdpOutPacket *out = createPacket(UDP_KEEPALIVE);
	*out << (uint32) rand();
	sendPacket(out);
}

uint16 UdpSession::sendPacket(UdpOutPacket *p)
{
	// Packet must be encrypted before sending to server
	if (p->cmd != UDP_NEW_UIN && p->cmd != UDP_LOGIN)
		p->encrypt();

	p->attempts = 0;
	p->expire = time(NULL) + SEND_TIMEOUT;
	sendDirect(p);
	sendQueue.push_back(p);
	return sendSeq;
}

/*
 * Utility function
 */
void UdpSession::getInfo(IcqInfo &info, UdpInPacket &in)
{
	in >> info.face >> info.nick >> info.age >> info.gender;
	in >> info.country >> info.province >> info.city;

	in >> info.email >> info.address >> info.zipcode >> info.tel;
	
	in >> info.name >> info.blood >> info.college;
	in >> info.profession >> info.homepage >> info.intro;
}

uint16 UdpSession::regNewUIN(const char *passwd)
{
	initSession();
	
	UdpOutPacket *out = createPacket(UDP_NEW_UIN);
	*out << passwd;
	return sendPacket(out);
}

uint16 UdpSession::getContactList()
{
	UdpOutPacket *out = createPacket(UDP_GET_CONTACTLIST);
	return sendPacket(out);
}

uint16 UdpSession::getRemoteContactList()
{
	UdpOutPacket *out = createPacket(UDP_GET_REMOTE_CONTACTLIST);
	return sendPacket(out);
}

uint16 UdpSession::login(const char *passwd, uint32 status)
{
	initSession();

	uint16 port = icqLink->createListenSession(TCP_SESSION_MSG);

	UdpOutPacket *out = createPacket(UDP_LOGIN);
	*out << passwd << status << (uint16) MYICQ_TCP_VER << realIP << port;
	return sendPacket(out);
}

void UdpSession::logout()
{
	UdpOutPacket *out = createPacket(UDP_LOGOUT);
	sendPacket(out);
	clearSendQueue();
}

void UdpSession::changeStatus(uint32 status)
{
	UdpOutPacket *out = createPacket(UDP_CHANGE_STATUS);
	*out << status;
	sendPacket(out);
}

/*
 * Retrieve a contact's information
 */
uint16 UdpSession::updateContact(QID &qid)
{
	UdpOutPacket *out = createPacket(UDP_UPDATE_CONTACT);
	*out << qid.uin << qid.domain;
	return sendPacket(out);
}

/*
 * Retrieve our information
 */
void UdpSession::updateUser()
{
	UdpOutPacket *out = createPacket(UDP_UPDATE_USER);
	sendPacket(out);
}

/*
 * Modify our personal information
 */
uint16 UdpSession::modifyUser(IcqUser *info, uint8 modifyPasswd)
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

uint16 UdpSession::sendMessage(uint8 type, QID &qid, const char *text)
{
	UdpOutPacket *out = createPacket(UDP_SEND_MSG);
	*out << type << qid.uin << text << qid.domain;
	return sendPacket(out);
}

/*
 * Send a broadcast message
 */
uint16 UdpSession::sendBCMsg(uint8 type, const char *text)
{
	UdpOutPacket *out = createPacket(UDP_BROADCAST_MSG);
	*out << type << text;

	return sendPacket(out);
}

uint16 UdpSession::getServerList()
{
	UdpOutPacket *out = createPacket(UDP_GET_SERVER_LIST);
	return sendPacket(out);
}

uint16 UdpSession::getGroupList(uint16 type)
{
	UdpOutPacket *out = createPacket(UDP_GET_GROUP_LIST);
	*out << type;
	return sendPacket(out);
}

uint16 UdpSession::searchGroup(uint32 id)
{
	UdpOutPacket *out = createPacket(UDP_SEARCH_GROUP);
	*out << id;
	return sendPacket(out);
}

uint16 UdpSession::searchRandom(const char *domain)
{
	UdpOutPacket *out = createPacket(UDP_SEARCH_RANDOM);
	*out << domain;
	return sendPacket(out);
}

uint16 UdpSession::searchUIN(QID &qid)
{
	UdpOutPacket *out = createPacket(UDP_SEARCH_CUSTOM);
	*out << qid.uin << qid.domain;
	return sendPacket(out);
}

uint16 UdpSession::searchCustom(const char *nick, const char *email, uint32 startUIN)
{
	UdpOutPacket *out = createPacket(UDP_SEARCH_CUSTOM);
	*out << (uint32) 0 << nick << email << startUIN;
	return sendPacket(out);
}

/*
 * Add a user to my contact list
 */
uint16 UdpSession::addFriend(QID &qid)
{
	UdpOutPacket *out = createPacket(UDP_ADD_FRIEND);
	*out << qid.uin << qid.domain;
	return sendPacket(out);
}

/*
 * Remove uin from my contact list
 */
uint16 UdpSession::delFriend(QID &qid)
{
	UdpOutPacket *out = createPacket(UDP_DEL_FRIEND);
	*out << qid.uin << qid.domain;
	return sendPacket(out);
}

uint16 UdpSession::createGroup(uint16 type, const char *name, const char *pass)
{
	UdpOutPacket *out = createPacket(UDP_CREATE_GROUP);
	*out << type << name << pass;
	return sendPacket(out);
}

uint16 UdpSession::enterGroup(uint32 id, const char *pass)
{
	UdpOutPacket *out = createPacket(UDP_ENTER_GROUP);
	*out << id << pass;
	return sendPacket(out);
}

void UdpSession::exitGroup(uint32 id)
{
	UdpOutPacket *out = createPacket(UDP_EXIT_GROUP);
	*out << id;
	sendPacket(out);
}

uint16 UdpSession::sendGroupMessage(uint32 id, const char *text)
{
	UdpOutPacket *out = createPacket(UDP_GROUP_MESSAGE);
	*out << id << text;
	return sendPacket(out);
}

void UdpSession::startGroup(uint32 id)
{
	UdpOutPacket *out = createPacket(UDP_GROUP_START);
	*out << id;
	sendPacket(out);
}

/*
 * Called when there is packet available in the system recv buffer
 */
bool UdpSession::onReceive()
{
	char data[MAX_PACKET_SIZE];
	char *p = data;
	sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	// Receive data from udp socket
	int n = recvfrom(sock, data, sizeof(data), 0, (sockaddr *) &addr, &addrlen);
	if (n < 0)
		return false;

	if (icqLink->isProxyType(PROXY_SOCKS)) {
		if (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 1)
			return false;

		p += 10;
		n -= 10;
	}

	UdpInPacket in(p, n);
	return onPacketReceived(in);
}

/*
 * Process a packet
 */
bool UdpSession::onPacketReceived(UdpInPacket &in)
{
	if (in.getSize() < sizeof(UDP_SRV_HDR))
		return false;

	if (in.header.ver != MYICQ_UDP_VER) {
		cout << "can not process packets of version " << in.header.ver << endl;
		return false;
	}

	// The session ID must match, otherwise, this is a cracking packet
	if (in.header.sid != sid) {
		cout << "packet does not belong to this session." << endl;
		return false;
	}

	ackseq = in.header.ackseq;
	uint16 seq = in.header.seq;

	if (ackseq) {
		if (onAck(ackseq) && !seq)
			icqLink->onAck(ackseq);
	}
	if (!seq)
		return true;

	if (!setWindow(seq)) {
		// This packet is a duplicated one that we have received already
		cout << "packet " << seq << " is duplicated" << endl;

		sendAckPacket(seq);
		return false;
	}

	uint16 cmd = in.getCmd();

	switch (cmd) {
	case UDP_NEW_UIN:
		onNewUINReply(in);
		break;

	case UDP_GET_CONTACTLIST:
		onContactListReply(in);
		break;

	case UDP_GET_REMOTE_CONTACTLIST:
		onRemoteContactList(in);
		break;

	case UDP_LOGIN:
		onLoginReply(in);
		break;

	case UDP_KEEPALIVE:
		in >> sessionCount;
		retryKeepAlive = 0;
		return true;

	case UDP_SRV_USER_ONLINE:
		onUserOnline(in);
		break;

	case UDP_SRV_USER_OFFLINE:
		onUserOffline(in);
		break;

	case UDP_SRV_MULTI_ONLINE:
		onMultiOnline(in);
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

	case UDP_GET_SERVER_LIST:
		onServerListReply(in);
		break;

	case UDP_SRV_GROUP_TYPES:
		onGroupTypes(in);
		break;

	case UDP_GET_GROUP_LIST:
		onGroupListReply(in);
		break;

	case UDP_SEARCH_GROUP:
		onSearchGroupReply(in);
		break;

	case UDP_CREATE_GROUP:
		onCreateGroupReply(in);
		break;

	case UDP_ENTER_GROUP:
		onEnterGroupReply(in);
		break;

	case UDP_GROUP_START:
		onGroupStart(in);
		break;

	case UDP_GROUP_MESSAGE:
		onGroupMessage(in);
		break;

	case UDP_GROUP_CMD:
		onGroupCmd(in);
		break;

	case UDP_SRV_ENTER_GROUP:
		onMemberEntered(in);
		break;

	case UDP_SRV_EXIT_GROUP:
		onMemberExited(in);
		break;

	default:
		return true;
	}

	sendAckPacket(seq);
	return true;
}

void UdpSession::onLoginReply(UdpInPacket &in)
{
	uint8 error;
	uint32 ip = 0;

	in >> error;
	if (error == LOGIN_SUCCESS)
		in >> ip >> sessionCount;

	icqLink->onLoginReply(error, ip);
}

/*
 * This packet is received during the login phase
 */
void UdpSession::onMultiOnline(UdpInPacket &in)
{
	uint16 num;
	QID qid;

	in >> num;

	while (num-- > 0) {
		uint32 status, ip, real_ip;
		uint16 port;
		in >> qid.uin >> status >> ip >> port >> real_ip;
		icqLink->onUserOnline(qid, status, ip, port, real_ip, true);
	}
}

void UdpSession::onUserOnline(UdpInPacket &in)
{
	QID qid;
	uint32 status, ip, real_ip;
	uint16 port;
	in >> qid.uin >> status >> ip >> port >> real_ip >> qid.domain;
	icqLink->onUserOnline(qid, status, ip, port, real_ip);
}

void UdpSession::onUserOffline(UdpInPacket &in)
{
	QID qid;
	in >> qid.uin >> qid.domain;
	icqLink->onUserOffline(qid);
}

void UdpSession::onStatusChanged(UdpInPacket &in)
{
	QID qid;
	uint32 status;
	in >> qid.uin >> status >> qid.domain;
	icqLink->onStatusChanged(qid, status);
}

void UdpSession::onNewUINReply(UdpInPacket &in)
{
	QID qid;
	in >> qid.uin >> qid.domain;
	icqLink->onNewUINReply(qid);
}

/*
 * This is a reply after a 'get contact list' request
 */
void UdpSession::onContactListReply(UdpInPacket &in)
{
	uint16 n;
	in >> n;

	QIDList l;
	while (n-- > 0) {
		QID qid;

		in >> qid.uin;
		l.push_back(qid);
	}
	icqLink->onContactListReply(l);
}

void UdpSession::onRemoteContactList(UdpInPacket &in)
{
	const char *domain;
	uint16 n;

	in >> domain >> n;

	QIDList l;
	while (n-- > 0) {
		QID qid;

		in >> qid.uin >> qid.domain;
		if (!qid.domain.empty())
			l.push_back(qid);
	}
	icqLink->onRemoteContactList(domain, l);
}

void UdpSession::onUpdateContactReply(UdpInPacket &in)
{
	IcqContact info;

	in >> info.qid.uin;
	getInfo(info, in);
	in >> info.qid.domain;

	icqLink->onUpdateContactReply(&info);
}

void UdpSession::onUpdateUserReply(UdpInPacket &in)
{
	IcqUser &info = icqLink->myInfo;
	getInfo(info, in);
	in >> info.auth;

	icqLink->onUpdateUserReply();
}

void UdpSession::onRecvMessage(UdpInPacket &in)
{
	uint8 type;
	QID from;
	uint32 when;
	const char *text;

	in >> type >> from.uin >> when >> text >> from.domain;
	icqLink->onRecvMessage(type, from, when, text, true);
}

void UdpSession::onSearchReply(UdpInPacket &in)
{
	PtrList *l = new PtrList;
	uint16 n;
	in >> n;

	while (n-- > 0) {
		SEARCH_RESULT *p = new SEARCH_RESULT;
		in >> p->uin >> p->online >> p->face >> p->nick >> p->province;
		l->push_back(p);
	}
	icqLink->onUserFound(l);
}

void UdpSession::onAddFriendReply(UdpInPacket &in)
{
	QID qid;
	uint8 result;
	in >> qid.uin >> result >> qid.domain;
	icqLink->onAddFriendReply(qid, result);
}

void UdpSession::onServerListReply(UdpInPacket &in)
{
	SERVER_INFO *info = NULL;

	uint16 n;
	in >> n;

	if (n)
		info = new SERVER_INFO[n];

	for (int i = 0; i < n; i++)
		in >> info[i].domain >> info[i].desc >> info[i].sessionCount;

	icqLink->onServerListReply(info, n);

	if (info)
		delete []info;
}

void UdpSession::onGroupTypes(UdpInPacket &in)
{
	uint16 n;
	in >> n;
	if (!n)
		return;

	PtrArray a;

	for (int i = 0; i < n; i++) {
		GROUP_TYPE_INFO *p = new GROUP_TYPE_INFO;
		in >> p->name >> p->displayName;
		a.push_back(p);
	}
	icqLink->onGroupTypes(a);
}

void UdpSession::onGroupListReply(UdpInPacket &in)
{
	PtrList l;

	uint16 n;
	in >> n;

	for (int i = 0; i < n; i++) {
		GROUP_INFO *p = new GROUP_INFO;
		in >> p->id >> p->name >> p->num;
		l.push_back(p);
	}

	icqLink->onGroupListReply(l);
}

void UdpSession::onSearchGroupReply(UdpInPacket &in)
{
	GROUP_INFO *info = new GROUP_INFO;

	in >> info->id >> info->name >> info->type >> info->num;
	icqLink->onSearchGroupReply(info);
}

void UdpSession::onCreateGroupReply(UdpInPacket &in)
{
	uint32 id;
	in >> id;

	icqLink->onCreateGroupReply(id);
}

void UdpSession::onEnterGroupReply(UdpInPacket &in)
{
	uint32 id;
	uint16 error;
	in >> id >> error;

	icqLink->onEnterGroupReply(id, error);

	if (error != GROUP_ERROR_SUCCESS)
		return;

	uint16 n;
	in >> n;
	for (int i = 0; i < n; i++) {
		uint32 uin;
		uint8 face;
		const char *nick;

		in >> uin >> face >> nick;
		icqLink->onMemberEntered(id, uin, face, nick);
	}
}

void UdpSession::onGroupStart(UdpInPacket &in)
{
	uint32 id;
	in >> id;

	icqLink->onGroupStart(id);
}

void UdpSession::onMemberEntered(UdpInPacket &in)
{
	uint32 id, uin;
	uint8 face;
	const char *nick;

	in >> id >> uin >> face >> nick;
	icqLink->onMemberEntered(id, uin, face, nick);
}

void UdpSession::onMemberExited(UdpInPacket &in)
{
	uint32 id, uin;

	in >> id >> uin;
	icqLink->onMemberExited(id, uin);
}

void UdpSession::onGroupMessage(UdpInPacket &in)
{
	uint32 id, from, when;
	const char *text;

	in >> id >> from >> when >> text;
	icqLink->onGroupMessage(id, from, when, text);
}

void UdpSession::onGroupCmd(UdpInPacket &in)
{
	uint32 id;

	in >> id;
	icqLink->onGroupCmd(id, in);
}

/*
 * Called when a proxy session has been initialized
 */
void UdpSession::onEstablished(bool success)
{
	if (success) {
		int type = icqLink->options.proxyType;
		int s = -1;

		if (type == PROXY_SOCKS)
			s = socksProxy.tcpSock;
		else if (type == PROXY_HTTP)
			s = httpProxy.sock;

		if (s >= 0) {
			sockaddr_in addr;
			socklen_t addrlen = sizeof(addr);
			getsockname(s, (sockaddr *) &addr, &addrlen);
			realIP = ntohl(addr.sin_addr.s_addr);
		}
	}

	icqLink->onConnect(success);
}
