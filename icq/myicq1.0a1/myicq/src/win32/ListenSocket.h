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

#if !defined(AFX_LISTENSOCKET_H__341221F2_4EB2_4E0E_B2AF_C26E8F4C2677__INCLUDED_)
#define AFX_LISTENSOCKET_H__341221F2_4EB2_4E0E_B2AF_C26E8F4C2677__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListenSocket.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListenSocket command target

class CListenSocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CListenSocket(const char *name);
	virtual ~CListenSocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListenSocket)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CListenSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	CString sessionName;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTENSOCKET_H__341221F2_4EB2_4E0E_B2AF_C26E8F4C2677__INCLUDED_)
