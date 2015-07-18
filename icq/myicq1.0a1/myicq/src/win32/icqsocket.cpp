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

#include "StdAfx.h"
#include "icqsocket.h"
#include "MySocket.h"
#include "ListenSocket.h"
#include <afxtempl.h>

static CMap<int, int, CAsyncSocket*, CAsyncSocket*> sockHash;


inline int addSocket(CAsyncSocket *pSocket)
{
	int sock = *pSocket;
	sockHash.SetAt(sock, pSocket);
	return sock;
}

void IcqSocket::destroy()
{
	POSITION pos = sockHash.GetStartPosition();
	while (pos) {
		int sock;
		CAsyncSocket *pSocket;
		sockHash.GetNextAssoc(pos, sock, pSocket);
		delete pSocket;
	}
	sockHash.RemoveAll();
}

int IcqSocket::createSocket(int type, SocketListener *l)
{
	CMySocket *pSocket = new CMySocket(l);
	pSocket->Create(0, type, FD_READ | FD_CONNECT | FD_CLOSE);
	return addSocket(pSocket);
}

int IcqSocket::createListenSocket(const char *name)
{
	CListenSocket *pSocket = new CListenSocket(name);
	pSocket->Create(0, SOCK_STREAM, FD_ACCEPT);
	pSocket->Listen();
	return addSocket(pSocket);
}

int IcqSocket::acceptSocket(int listenSock, SocketListener *l)
{
	CAsyncSocket *pListenSocket;
	if (sockHash.Lookup(listenSock, pListenSocket)) {
		CMySocket *pSocket = new CMySocket(l);
		pListenSocket->Accept(*pSocket);
		pSocket->AsyncSelect(FD_READ | FD_CLOSE);
		return addSocket(pSocket);
	}
	return -1;
}

void IcqSocket::closeSocket(int sock)
{
	CAsyncSocket *pSocket;
	if (sockHash.Lookup(sock, pSocket)) {
		sockHash.RemoveKey(sock);
		delete pSocket;
	}
}

void IcqSocket::enableWrite(int sock)
{
	CAsyncSocket *pSocket;
	if (sockHash.Lookup(sock, pSocket))
		pSocket->AsyncSelect(FD_WRITE | FD_READ | FD_CLOSE);
}

int IcqSocket::getSocketError()
{
	return WSAGetLastError();
}