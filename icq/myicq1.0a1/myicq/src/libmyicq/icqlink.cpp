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
#include "udpsession.h"
#include "msgsession.h"
#include "tcpsession.h"
#include "icqgroup.h"
#include "icqplugin.h"
#include "groupplugin.h"
#include "ndes.h"
#include <time.h>

IcqLink *icqLink;


IcqLink::IcqLink()
{
	icqLink = this;

	srand(time(NULL));
	desinit(0);

	PluginFactory::init();

	ourIP = 0;
	udpSession = new UdpSession(this);
}

IcqLink::~IcqLink()
{
	desdone();
	destroyUser();
	if (udpSession)
		delete udpSession;
	IcqSocket::destroy();
	PluginFactory::destroy();
}

GROUP_TYPE_INFO *IcqLink::getGroupTypeInfo(int type)
{
	if (type < 0 || type >= groupTypes.size())
		return NULL;
	return (GROUP_TYPE_INFO *) groupTypes[type];
}

ContactInfo *IcqLink::getContactInfo(QID *qid)
{
	if (qid)
		return findContact(*qid);
	return &myInfo;
}

TcpSessionBase *IcqLink::createTcpSession(TcpSessionListener *l, uint32 ip, uint16 port)
{
	// Anonymous tcp session, do not link it into the list
	TcpSession *s = createTcpSession("", QID());
	s->listener = l;
	s->connect(ip, port, true);
	return s;
}

void IcqLink::logout()
{
	if (myInfo.status != STATUS_OFFLINE) {
		udpSession->logout();
		myInfo.status = STATUS_OFFLINE;
	}

	destroyTcpSession();

	PtrList::iterator it;
	for (it = contactList.begin(); it != contactList.end(); ++it) {
		IcqContact *c = (IcqContact *) *it;
		c->status = STATUS_OFFLINE;
		c->ip = c->realIP = 0;
		c->port = 0;
	}
}

bool IcqLink::registerExePlugin(const char *path, const char *name)
{
	myProfile.setFileName(path);
	myProfile.setSectionName(name);
	const char *cmd = myProfile.readString("server", NULL);
	if (!cmd)
		return false;

	ExePlugin *p = new ExePlugin;
	p->name = name;
	p->info.type = ICQ_PLUGIN_EXE;
	p->info.icon = NULL;
	p->info.name = myProfile.readString("name", name);
	p->serverCmd = cmd;
	p->clientCmd = myProfile.readString("client", cmd);
	return PluginFactory::registerPlugin(p);
}

void IcqLink::initUser(const char *dir)
{
	char pass[8];
	strncpy(pass, myInfo.passwd.c_str(), sizeof(pass));
	setkey(pass);

	string name = dir;
	name += "plugins.cfg";
	myProfile.setFileName(name.c_str());
	
	PtrList &l = PluginFactory::getPluginList();
	PtrList::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		IcqPlugin *p = (IcqPlugin *) *it;
		if (p->info.type == ICQ_PLUGIN_TOY) {
			myProfile.setSectionName(p->name.c_str());
			((ToyPlugin *) p)->init(icqLink, &myProfile);
		}
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

void IcqLink::destroyTcpSession(QID &qid)
{
	PtrList::iterator it, next;
	for (it = tcpSessionList.begin(); it != tcpSessionList.end(); it = next) {
		next = it;
		++next;
		TcpSession *s = (TcpSession *) *it;
		if (s->qid == qid) {
			s->deleteIt();
			tcpSessionList.erase(it);
		}
	}
}

void IcqLink::destroyTcpSession()
{	
	PtrList::iterator it, next;
	for (it = tcpSessionList.begin(); it != tcpSessionList.end(); it = next) {
		next = it;
		++next;
		TcpSession *s = (TcpSession *) *it;
		if (s->qid.uin || myInfo.status == STATUS_OFFLINE) {
			s->deleteIt();
			tcpSessionList.erase(it);
		}
	}
}

void IcqLink::destroyContact(QID &qid)
{
	destroyTcpSession(qid);

	PtrList l;
	while (!windowList.empty()) {
		IcqWindow *win = (IcqWindow *) windowList.front();
		windowList.pop_front();
		if (win->qid == qid)
			delete win;
		else
			l.push_back(win);
	}

	windowList = l;
}

void IcqLink::removeTcpSession(TcpSession *s)
{
	PtrList::iterator it;
	for (it = tcpSessionList.begin(); it != tcpSessionList.end(); ++it) {
		if (s == (TcpSession *) *it) {
			tcpSessionList.erase(it);
			break;
		}
	}
}

void IcqLink::onUserOffline(QID &qid)
{
	IcqContact *c = findContact(qid, CONTACT_FRIEND);
	if (c) {
		c->ip = 0;
		c->port = 0;
		c->status = STATUS_OFFLINE;
	}
	destroyTcpSession(qid);
}

/*
 * Create a new tcp session, and put it into the front of the queue
 */
TcpSession *IcqLink::createTcpSession(const char *name, QID &qid)
{
	TcpSession *s = new TcpSession(this, name, qid);
	if (s)
		tcpSessionList.push_front(s);
	return s;
}

TcpSession *IcqLink::acceptSession(int listenSock, const char *name)
{
	TcpSession *s = createTcpSession(name, QID());
	s->status = TCP_STATUS_HELLO_WAIT;
	s->sock = IcqSocket::acceptSocket(listenSock, s);
	if (!strcmp(name, TCP_SESSION_MSG))
		s->listener = new MsgSession(s);
	return s;
}

uint16 IcqLink::createListenSession(const char *name)
{
	QID qid;
	TcpSession *s = findTcpSession(name, qid);
	if (!s) {
		s = (TcpSession *) createTcpSession(name, qid);
		s->sock = IcqSocket::createListenSocket(name);
	}

	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	if (getsockname(s->sock, (sockaddr *) &addr, &addrLen) < 0)
		return 0;

	return ntohs(addr.sin_port);
}

IcqGroup *IcqLink::createGroup(const char *plugin, uint32 id)
{
	if (!id)
		return NULL;
	IcqGroup *g = findGroup(id);
	if (g)
		return NULL;

	GroupPlugin *p = GroupPlugin::get(plugin);
	if (p) {
		g = new IcqGroup(p, id);
		groupList.push_back(g);
	}
	return g;
}

uint32 IcqLink::sendMessage(uint8 type, QID &to, const char *text, bool relay)
{
	if (!relay) {
		TcpSession *s = findTcpSession(TCP_SESSION_MSG, to);
		if (!s) {
			IcqContact *c = findContact(to);
			if (c && c->ip) {
				s = createTcpSession(TCP_SESSION_MSG, to);
				s->listener = new MsgSession(s);
				s->connect(c->ip, c->port, true);
			}
		}
		if (s && s->status != TCP_STATUS_DISABLED) {
			MsgSession *session = (MsgSession *) s->listener;
			return session->sendMessage(type, text);
		}
	}
	return udpSession->sendMessage(type, to, text);
}

uint32 IcqLink::sendMessage(IcqMsg &msg)
{
	TextOutStream out;
	msg.encode(out);

	bool relay = ((msg.flags & MF_RELAY) ? true : false);
	return sendMessage(msg.type, msg.qid, out, relay);
}

void IcqLink::cancelSendMessage(QID &qid)
{
	TcpSession *s = findTcpSession(TCP_SESSION_MSG, qid);
	if (s)
		s->clearSendQueue();
}

uint32 IcqLink::sendTcpRequest(const char *name, IcqContact *c, const char *text)
{
	uint16 port = 0;

	// If the remote is behind a firewall, establish a reverse connection
	if (c->ip != c->realIP && c->ip != ourIP)
		port = createListenSession(name);

	TextOutStream out;
	out << name << text << port;
	return sendMessage(MSG_TCP_REQUEST, c->qid, out);
}

void IcqLink::acceptTcpRequest(const char *name, IcqContact *c, uint16 port)
{
	IcqPlugin *p = PluginFactory::getPlugin(name);
	if (!p)
		return;

	int type = p->info.type;
	if (type == ICQ_PLUGIN_NET) {
		if (port) {
			TcpSession *s = (TcpSession *) createTcpSession(name, c->qid);
			s->connect(c->ip, port, false);
			port = 0;
		} else
			port = createListenSession(name);

	} else if (type == ICQ_PLUGIN_EXE) {
		((ExePlugin *) p)->execServer(c);
	} else
		return;

	TextOutStream out;
	out << name << port;
	sendMessage(MSG_TCP_ACCEPTED, c->qid, out);
}

IcqContact *IcqLink::findContact(QID &qid, int type)
{
	PtrList::iterator it;
	for (it = contactList.begin(); it != contactList.end(); ++it) {
		IcqContact *c = (IcqContact *) *it;
		if (c->qid == qid && (type == -1 || c->type == type))
			return c;
	}
	return NULL;
}

IcqGroup *IcqLink::findGroup(uint32 id)
{
	PtrList::iterator it;
	for (it = groupList.begin(); it != groupList.end(); ++it) {
		IcqGroup *g = (IcqGroup *) *it;
		if (g->id == id)
			return g;
	}
	return NULL;
}

IcqWindow *IcqLink::findWindow(int type, QID *qid, uint32 seq)
{
	PtrList::iterator it;
	for (it = windowList.begin(); it != windowList.end(); ++it) {
		IcqWindow *win = (IcqWindow *) *it;
		if (win->type == type &&
			(!qid || win->qid == *qid) &&
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

GroupWindow *IcqLink::findGroupWindow(uint32 id)
{
	PtrList::iterator it;
	for (it = groupWindowList.begin(); it != groupWindowList.end(); ++it) {
		GroupWindow *win = (GroupWindow *) *it;
		if (win->id == id)
			return win;
	}
	return NULL;
}

GroupWindow *IcqLink::findGroupWindowSeq(uint32 seq)
{
	PtrList::iterator it;
	for (it = groupWindowList.begin(); it != groupWindowList.end(); ++it) {
		GroupWindow *win = (GroupWindow *) *it;
		if (win->seq == seq)
			return win;
	}
	return NULL;
}

TcpSession *IcqLink::findTcpSession(const char *name, QID &qid)
{
	PtrList::iterator it;
	for (it = tcpSessionList.begin(); it != tcpSessionList.end(); ++it) {
		TcpSession *session = (TcpSession *) *it;
		if ((session->qid == qid) && session->name.compare(name) == 0)
			return session;
	}
	return NULL;
}

IcqMsg *IcqLink::findPendingMsg(QID *qid)
{
	if (msgList.empty())
		return NULL;

	IcqMsg *msg;

	if (qid) {
		PtrList::iterator i;
		for (i = msgList.begin(); i != msgList.end(); i++) {
			msg = (IcqMsg *) *i;
			if (!msg->isSysMsg() && msg->qid == *qid)
				return msg;
		}
	} else {
		msg = (IcqMsg *) msgList.front();
		if (msg->isSysMsg())
			return msg;
	}
	return NULL;
}

IcqMsg *IcqLink::findPendingMsg(uint32 id)
{
	PtrList::iterator i;
	for (i = msgList.begin(); i != msgList.end(); i++) {
		IcqMsg *msg = (IcqMsg *) *i;
		if (msg->id == id)
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

void IcqLink::changeStatus(int status)
{
	if (status == STATUS_INVIS)
		destroyTcpSession();
	udpSession->changeStatus(status);
}

void IcqLink::onAck(uint32 seq)
{
	IcqWindow *win = findWindowSeq(seq);
	if (win) {
		win->onAck(seq);
		return;
	}

	GroupWindow *gw = findGroupWindowSeq(seq);
	if (gw)
		gw->onAck();
}

void IcqLink::onSendError(uint32 seq)
{
	IcqWindow *win = findWindowSeq(seq);
	if (win) {
		win->onSendError(seq);
		return;
	}

	GroupWindow *gw = findGroupWindowSeq(seq);
	if (gw)
		gw->onSendError();
}

void IcqLink::onUpdateContactReply(IcqContact *info)
{
	IcqContact *c = findContact(info->qid);
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

void IcqLink::onRecvMessage(uint8 type, QID &from, uint32 when, const char *text, bool relay)
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
				int type = p->info.type;
				if (type == ICQ_PLUGIN_NET) {
					if (port) {
						TcpSession *s = (TcpSession *) createTcpSession(name.c_str(), from);
						s->connect(c->ip, port, true);
					}
				} else if (type == ICQ_PLUGIN_EXE)
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
	msg->qid = from;
	msg->when = when;
	msg->decode(text);

	onRecvMessage(msg);
}

void IcqLink::onMemberEntered(uint32 id, uint32 uin, uint8 face, const char *nick)
{
	IcqGroup *g = findGroup(id);
	if (g)
		g->addMember(uin, face, nick);
}

void IcqLink::onMemberExited(uint32 id, uint32 uin)
{
	IcqGroup *g = findGroup(id);
	if (g)
		g->removeMember(uin);
}

void IcqLink::onGroupTypes(PtrArray &a)
{
	groupTypes = a;
}

void IcqLink::exitGroup(IcqGroup *g)
{
	udpSession->exitGroup(g->id);
	groupList.remove(g);

	PtrList l;
	while (!groupWindowList.empty()) {
		GroupWindow *win = (GroupWindow *) groupWindowList.front();
		groupWindowList.pop_front();
		if (win->id == g->id)
			delete win;
		else
			l.push_back(win);
	}
	groupWindowList = l;

	delete g;
}

void IcqLink::onGroupStart(uint32 id)
{
	IcqGroup *g = findGroup(id);
	if (g)
		g->onStart();
}

void IcqLink::onGroupCmd(uint32 id, InPacket &in)
{
	IcqGroup *g = findGroup(id);
	if (g)
		g->onGroupCmd(in);
}

void IcqLink::onGroupMessage(uint32 id, uint32 from, uint32 when, const char *text)
{
	IcqGroup *g = findGroup(id);
	if (!g || g->msgOption == GROUP_OPTION_MSG_IGNORE)
		return;

	IcqMsg *msg = new IcqMsg;

	msg->type = MSG_TEXT;
	msg->id = id;
	msg->flags = MF_RECEIVED;
	msg->qid.uin = from;
	msg->when = when;
	msg->decode(text);

	onGroupMessage(msg);
}
