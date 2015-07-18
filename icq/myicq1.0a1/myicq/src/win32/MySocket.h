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

#if !defined(AFX_MYSOCKET_H__D3BD218F_19E6_47E1_97BC_0391F0A81230__INCLUDED_)
#define AFX_MYSOCKET_H__D3BD218F_19E6_47E1_97BC_0391F0A81230__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MySocket.h : header file
//

class SocketListener;

/////////////////////////////////////////////////////////////////////////////
// CMySocket command target

class CMySocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CMySocket(SocketListener *l);
	virtual ~CMySocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMySocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
private:
	SocketListener *listener;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSOCKET_H__D3BD218F_19E6_47E1_97BC_0391F0A81230__INCLUDED_)
