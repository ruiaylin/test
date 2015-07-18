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

#include "icqlink.h"
#include "icqwindow.h"
#include "icqsocket.h"
#include "serversession.h"
#include "msgsession.h"
#include "tcpsession.h"
#include "icqdb.h"
#include "icqplugin.h"
#include "ndes.h"
#include <time.h>

IcqLink *icqLink;

static bool isUdpSession(IcqSession *s)
{
	return (s->name.compare(ICQ_SESSION_SERVER) == 0 ||
			s->name.compare(ICQ_SESSION_MSG) == 0);
}


IcqLink::IcqLink()
{
	icqLink = this;

	srand(time(NULL));
	desinit(0);

	PluginFactory::init();

	socksIP = 0;
	createSession(ICQ_SESSION_SERVER, 0);
}

IcqLink::~IcqLink()
{
	desdone();
	destroyUser();
	delete serverSession();
	IcqSocket::destroy();
	PluginFactory::destroy();
}

ContactInfo *IcqLink::getContactInfo(uint32 uin)
{
	if (uin)
		return findContact(uin);
	return &myInfo;
}

TcpSessionBase *IcqLink::createTcpSession(TcpSessionListener *l, uint32 ip, uint16 port)
{
	// Anonymous tcp session
	TcpSession *s = new TcpSession(this, "", 0);
	s->setListener(l);
	s->connect(ip, port, TRUE);
	return s;
}

void IcqLink::checkSendQueue()
{
	PtrList::iterator it;
	for (it = sessionList.begin(); it != sessionList.end(); ++it) {
		if (isUdpSession((IcqSession *) *it))
			((UdpSession *) *it)->checkSendQueue();
	}
}

void IcqLink::logout()
{
	if (myInfo.status != STATUS_OFFLINE) {
		serverSession()->logout();
		myInfo.status = STATUS_OFFLINE;
	}

	destroySession();

	PtrList::iterator it;
	for (it = contactList.begin(); it != contactList.end(); ++it) {
		IcqContact *c = (IcqContact *) *it;
		c->status = STATUS_OFFLINE;
		c->ip = 0;
		c->port = 0;
	}
}

void IcqLink::destroyUser()
{
	logout();

	while (!windowList.empty())
		delete (IcqWindow *) windowList.front();
	
	PtrList::iterator it;
	for (it = contactList.begin(); it != contactList.end(); ++it)
		delete (IcqContact *) *it;
	contactList.clear();
	
	for (it = msgList.begin(); it != msgList.end(); ++it)
		delete (IcqMsg *) *it;
	msgList.clear();

	myInfo.status = STATUS_OFFLINE;
}

void IcqLink::destroySession(uint32 uin)
{
	PtrList::iterator it, next;
	for (it = sessionList.begin(); it != sessionList.end(); it = next) {
		next = it;
		++next;
		IcqSession *session = (IcqSession *) *it;
		if (session->uin == uin) {
			delete session;
			sessionList.erase(it);
		}
	}
}

void IcqLink::destroySession()
{	
	int n = sessionList.size();
	while (n-- > 1) {
		delete (IcqSession *) sessionList.back();
		sessionList.pop_back();
	}
}

void IcqLink::destroyContact(uint32 uin)
{
	destroySession(uin);
	
	PtrList l;
	while (!windowList.empty()) {
		IcqWindow *win = (IcqWindow *) windowList.front();
		windowList.pop_front();
		if (win->uin == uin)
			delete win;
		else
			l.push_back(win);
	}
	windowList = l;
}

void IcqLink::destroySession(const char *name, uint32 uin)
{
	PtrList::iterator it;
	for (it = sessionList.begin(); it != sessionList.end(); ++it) {
		IcqSession *s = (IcqSession *) *it;
		if (s->uin == uin && s->name.compare(name) == 0) {
			delete s;
			sessionList.erase(it);
			break;
		}
	}
}

void IcqLink::onUserOffline(uint32 uin)
{
	IcqContact *c = findContact(uin, CONTACT_FRIEND);
	if (c) {
		c->ip = 0;
		c->port = 0;
		c->status = STATUS_OFFLINE;
	}
	destroySession(uin);
}

IcqSession *IcqLink::createSession(const char *name, uint32 uin)
{
	IcqSession *s = NULL;

	if (strcmp(name, ICQ_SESSION_SERVER) == 0)
		s = new ServerSession(this);
	else if (strcmp(name, ICQ_SESSION_MSG) == 0)
		s = new MsgSession(this, uin);
	else
		s = new TcpSession(this, name, uin);

	if (s)
		sessionList.push_back(s);
	return s;
}

TcpSession *IcqLink::acceptSession(int listenSock, const char *name)
{
	TcpSession *s = (TcpSession *) createSession(name, 0);
	s->status = TCP_STATUS_HELLO_WAIT;
	s->sock = IcqSocket::acceptSocket(listenSock, s);
	return s;
}

uint16 IcqLink::createListenSession(const char *name)
{
	TcpSession *s = (TcpSession *) findSession(name, 0);
	if (!s) {
		s = (TcpSession *) createSession(name, 0);
		s->sock = IcqSocket::createListenSocket(name);
	}

	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	if (getsockname(s->sock, (sockaddr *) &addr, &addrLen) < 0)
		return 0;

	return ntohs(addr.sin_port);
}

uint32 IcqLink::sendMessage(uint8 type, uint32 to, const char *text)
{
	MsgSession *s = (MsgSession *) findSession(ICQ_SESSION_MSG, to);
	if (!s) {
		IcqContact *c = findContact(to);
		if (c) {
			if (c->ip == 0 || c->port == 0)
				return serverSession()->sendMessage(type, to, text);

			s = (MsgSession *) createSession(ICQ_SESSION_MSG, to);
			s->connect(c->ip, c->port);
		}
	}
	if (s)
		return s->sendMessage(type, text);
	return 0;
}

uint32 IcqLink::sendTcpRequest(const char *name, IcqContact *c, const char *text)
{
	uint16 port = 0;
	if (c->realIP && c->ip != socksIP)
		port = createListenSession(name);

	TextOutStream out;
	out << name << text << port;
	return sendMessage(MSG_TCP_REQUEST, c->uin, out);
}

void IcqLink::acceptTcpRequest(const char *name, IcqContact *c, uint16 port)
{
	IcqPlugin *p = PluginFactory::getPlugin(name);
	if (!p)
		return;

	if (p->type == ICQ_PLUGIN_NET) {
		if (port) {
			TcpSession *s = (TcpSession *) createSession(name, c->uin);
			s->connect(c->ip, port, false);
			port = 0;
		} else
			port = createListenSession(name);

	} else if (p->type == ICQ_PLUGIN_EXE) {
		((ExePlugin *) p)->execServer(c);
	} else
		return;

	TextOutStream out;
	out << name << port;
	sendMessage(MSG_TCP_ACCEPTED, c->uin, out);
}

IcqContact *IcqLink::findContact(uint32 uin, int type)
{
	PtrList::iterator it;
	for (it = contactList.begin(); it != contactList.end(); ++it) {
		IcqContact *c = (IcqContact *) *it;
		if (c->uin == uin && (type == -1 || c->type == type))
			return c;
	}
	return NULL;
}

IcqWindow *IcqLink::findWindow(int type, uint32 uin, uint32 seq)
{
	PtrList::iterator it;
	for (it = windowList.begin(); it != windowList.end(); ++it) {
		IcqWindow *win = (IcqWindow *) *it;
		if (win->type == type &&
			(uin == 0 || win->uin == uin) &&
			(seq == 0 || win->seq == seq))
			return win;
	}
	return NULL;
}

IcqWindow *IcqLink::findWindowSeq(uint32 seq)
{
	PtrList::iterator it;
	for (it = windowList.begin(); it != windowList.end(); ++it) {
		IcqWindow *win = (IcqWindow *) *it;
		if (win->isSeq(seq))
			return win;
	}
	return NULL;
}

IcqSession *IcqLink::findSession(const char *name, uint32 uin)
{
	PtrList::iterator it;
	for (it = sessionList.begin(); it != sessionList.end(); ++it) {
		IcqSession *session = (IcqSession *) *it;
		if ((!uin || session->uin == uin) && session->name.compare(name) == 0)
			return session;
	}
	return NULL;
}

IcqMsg *IcqLink::findPendingMsg(uint32 uin)
{
	if (msgList.empty())
		return NULL;

	IcqMsg *msg;

	if (uin) {
		PtrList::iterator i;
		for (i = msgList.begin(); i != msgList.end(); i++) {
			msg = (IcqMsg *) *i;
			if (!msg->isSysMsg() && msg->uin == uin)
				return msg;
		}
	} else {
		msg = (IcqMsg *) msgList.front();
		if (msg->isSysMsg())
			return msg;
	}
	return NULL;
}

void IcqLink::addPendingMsg(IcqMsg *msg)
{	
	if (!msg->isSysMsg())
		msgList.push_back(msg);
	else {

	    PtrList::iterator i;
	    for (i = msgList.begin(); i != msgList.end(); i++) {
			IcqMsg *p = (IcqMsg *) *i;
			if (!p->isSysMsg())
				break;
		}
		msgList.insert(i, msg);
	}
}

void IcqLink::onAck(uint32 seq)
{
	IcqWindow *win = findWindowSeq(seq);
	if (win)
		win->onAck(seq);
}

void IcqLink::onSendError(uint32 seq)
{
	IcqWindow *win = findWindowSeq(seq);
	if (win)
		win->onSendError(seq);
}

void IcqLink::onUpdateContactReply(IcqContact *info)
{
	IcqContact *c = findContact(info->uin);
	if (c) {
		c->face = info->face;
		c->nick = info->nick;
		c->age = info->age;
		c->gender = info->gender;
		c->country = info->country;
		c->province = info->province;
		c->city = info->city;

		c->email = info->email;
		c->address = info->address;
		c->zipcode = info->zipcode;
		c->tel = info->tel;

		c->name = info->name;
		c->blood = info->blood;
		c->college = info->college;
		c->profession = info->profession;
		c->homepage = info->homepage;
		c->intro = info->intro;
	}
}

void IcqLink::onRecvMessage(uint8 type, uint32 from, uint32 when, const char *text, bool relay)
{
	IcqContact *c = findContact(from);
	if (c) {
		if (c->type == CONTACT_IGNORE || c->flags.test(CF_OPTION_IGNORE))
			return;

		if (type == MSG_TCP_ACCEPTED) {
			TextInStream in(text);
			string name;
			uint16 port;
			in >> name >> port;

			IcqPlugin *p = PluginFactory::getPlugin(name);
			if (p) {
				if (p->type == ICQ_PLUGIN_NET) {
					if (port) {
						TcpSession *s = (TcpSession *) createSession(name.c_str(), from);
						s->connect(c->ip, port, true);
					}
				} else if (p->type == ICQ_PLUGIN_EXE)
					((ExePlugin *) p)->execClient(c);
			}
			return;
		}
	}

	IcqMsg *msg = new IcqMsg;
	msg->type = type;
	msg->flags = MF_RECEIVED;
	if (relay)
		msg->flags |= MF_RELAY;
	msg->uin = from;
	msg->when = when;
	msg->text = text;

	onRecvMessage(msg);
}
