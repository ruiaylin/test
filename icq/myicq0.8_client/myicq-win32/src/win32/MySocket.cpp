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

// MySocket.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "socketlistener.h"
#include "MySocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySocket

CMySocket::CMySocket(SocketListener *l)
{
	listener = l;
}

CMySocket::~CMySocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMySocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CMySocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CMySocket member functions

void CMySocket::OnReceive(int nErrorCode) 
{
	listener->onReceive();
}

void CMySocket::OnConnect(int nErrorCode) 
{
	listener->onConnect(nErrorCode == 0);
}

void CMySocket::OnSend(int nErrorCode) 
{
	listener->onSend();
}

void CMySocket::OnClose(int nErrorCode) 
{
	listener->onClose();
}
