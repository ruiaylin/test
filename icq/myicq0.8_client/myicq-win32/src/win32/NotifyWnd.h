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

#if !defined(AFX_NOTIFYWND_H__F72D28F1_212A_4768_A847_961FE49271BA__INCLUDED_)
#define AFX_NOTIFYWND_H__F72D28F1_212A_4768_A847_961FE49271BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NotifyWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNotifyWnd window

class CNotifyWnd : public CWnd
{
// Construction
public:
	CNotifyWnd(HICON icon, const char *text, DWORD t = 3000);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotifyWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNotifyWnd();

private:
	HICON hIcon;
	CString strText;
	COLORREF clrBackground, clrText;
	int wndWidth, wndHeight;
	CRect rcWorkArea;
	DWORD waitTime;

	static CNotifyWnd *notifyWnd;

	// Generated message map functions
protected:
	//{{AFX_MSG(CNotifyWnd)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTIFYWND_H__F72D28F1_212A_4768_A847_961FE49271BA__INCLUDED_)
