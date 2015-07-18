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

#define MYICQ_UDP_VER			1
#define MAX_SEND_ATTEMPTS		2


int UdpSession::udpSock = -1;
string UdpSession::destHost;
sockaddr_in UdpSession::proxyAddr;


UdpSession::UdpSession(IcqLink *link, const char *name, uint32 uin)
: IcqSession(link, name, uin)
{
	memset(&destAddr, 0, sizeof(destAddr));

	initSession();
}

UdpSession::~UdpSession()
{
	clearSendQueue();
}

void UdpSession::connect(uint32 ip, uint16 port)
{
	memset(&destAddr, 0, sizeof(destAddr));
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = htonl(ip);
	destAddr.sin_port = htons(port);
}

time_t UdpSession::checkSendQueue()
{
	time_t now = time(NULL);

	while (!sendQueue.empty()) {
		UdpOutPacket *p = (UdpOutPacket *) sendQueue.front();
		if (p->expire > now)
			return (p->expire - now);

		sendQueue.pop_front();
		p->attempts++;
		if (p->attempts <= MAX_SEND_ATTEMPTS) {
			cout << "retransmit packet " << p->seq << endl;

			p->expire = now + SEND_TIMEOUT;
			sendDirect(p);
			sendQueue.push_back(p);
		}
		else {
			cout << "packet " << p->seq << " time out" << endl;
			
			onSendError(p);
			delete p;
		}
	}
	return SEND_TIMEOUT;
}

void UdpSession::clearSendQueue()
{
	PtrList::iterator i;
	for (i = sendQueue.begin(); i != sendQueue.end(); i++)
		delete (UdpOutPacket *) *i;
	sendQueue.clear();
}

void UdpSession::initSession()
{
	sid = (rand() & 0x7fffffff) + 1;
	sendSeq = (rand() & 0x3fff);
	recvSeq = 0;
	window = 0;

	clearSendQueue();
}

void UdpSession::createPacket(UdpOutPacket &out, uint16 cmd, uint16 seq, uint16 ackseq)
{
	IcqOption &options = icqLink->options;

	if (options.flags.test(UF_USE_PROXY)) {
		switch (options.proxyType) {
		case PROXY_HTTP:
			out << (uint16) 0;
			break;

		case PROXY_SOCKS:
			out << (uint16) 0 << (uint8) 0;
			if (options.proxy[PROXY_SOCKS].resolve) {
				out << (uint8) 1;
				out.writeData((const char *) &destAddr.sin_addr.s_addr, 4);
			} else {
				uint8 len = destHost.length();
				out << (uint8) 3 << len;
				out.writeData(destHost.c_str(), len);
			}
			out.writeData((const char *) &destAddr.sin_port, 2);
			break;
		}
	}
	out << (uint16) MYICQ_UDP_VER << (uint32) 0;
	out << sid << seq << (uint16) ackseq << cmd << icqLink->myInfo.uin;
	out.beginData();
}

UdpOutPacket *UdpSession::createPacket(uint16 cmd, uint16 ackseq)
{
	UdpOutPacket *p = new UdpOutPacket;
	p->cmd = cmd;
	p->seq = ++sendSeq;
	createPacket(*p, cmd, sendSeq, ackseq);
	return p;
}

void UdpSession::sendAckPacket(uint16 seq)
{
	UdpOutPacket out;
	createPacket(out, UDP_ACK, seq);
	sendDirect(&out);
}

void UdpSession::sendDirect(UdpOutPacket *p, int s, sockaddr_in *addr)
{
	sendto(s, p->getData(), p->getSize(), 0, (sockaddr *) addr, sizeof(sockaddr_in));
}

void UdpSession::sendDirect(UdpOutPacket *p)
{
	if (icqLink->isProxyType(PROXY_SOCKS))
		sendDirect(p, udpSock, &proxyAddr);
	else
		sendDirect(p, udpSock, &destAddr);
}

void UdpSession::onSendError(UdpOutPacket *p)
{
	icqLink->onSendError(p->seq);
}

uint16 UdpSession::sendPacket(UdpOutPacket *p)
{
	p->attempts = 0;
	p->expire = time(NULL) + SEND_TIMEOUT;
	sendDirect(p);
	sendQueue.push_back(p);
	return sendSeq;
}

bool UdpSession::setWindow(uint16 seq)
{
	if (seq >= recvSeq + 32 || seq < recvSeq)
		return false;

	if (seq == recvSeq) {
		do {
			recvSeq++;
			window >>= 1;
		} while (window & 0x1);
	} else {
		uint32 mask = (1 << (seq - recvSeq));
		if (window & mask)
			return false;
		else
			window |= mask;
	}
	return true;
}

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

bool UdpSession::onPacketReceived(UdpInPacket &in)
{
	if (in.getVersion() != MYICQ_UDP_VER) {
		cout << "malformed packet." << endl;
		return false;
	}

	if (in.getSID() != sid) {
		cout << "packet does not belong to this session." << endl;
		return false;
	}

	uint16 cmd = in.getCmd();
	uint16 ackseq = in.getAckSeq();
	uint16 seq = in.getSeq();

	if (ackseq)
		onAck(ackseq);
	if (!seq)
		return true;

	if (!recvSeq)
		recvSeq = seq;
	if (!setWindow(seq)) {
		cout << "packet " << seq << " is duplicated" << endl;
		return false;
	}
	return true;
}

bool UdpSession::onReceive()
{
	char data[MAX_PACKET_SIZE];
	char *p = data;
	sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int n = ::recvfrom(udpSock, data, sizeof(data), 0, (sockaddr *) &addr, &addrlen);
	if (n < 0) {
		icqLink->onSendError(0);
		return false;
	}
	if (icqLink->isProxyType(PROXY_SOCKS)) {
		if (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 1)
			return false;

		p += 10;
		n -= 10;
	}

	if (n < (int) sizeof(UDP_HEADER)) {
		cout << "packet size is too small" << endl;
		return false;
	}

	UdpInPacket in(p, n);
	uint16 cmd = in.getCmd();
	uint32 uin = in.getUIN();
	const char *name;

	if (cmd >= UDP_MSG_FIRST)
		name = ICQ_SESSION_MSG;
	else {
		name = ICQ_SESSION_SERVER;
		uin = 0;
	}

	UdpSession *s = (UdpSession *) icqLink->findSession(name, uin);
	if (!s && strcmp(name, ICQ_SESSION_SERVER) != 0) {
		s = (UdpSession *) icqLink->createSession(name, uin);
		s->sid = in.getSID();

		if (icqLink->isProxyType(PROXY_SOCKS)) {
			sockaddr_in &addr = s->destAddr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr = *(in_addr *) &data[4];
			addr.sin_port = *(uint16 *) &data[8];
		} else
			s->destAddr = addr;
	}
	if (!s)
		return false;

	return s->onPacketReceived(in);
}
