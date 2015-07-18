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

#if !defined(AFX_RICHEDITCTRLEX_H__5C82606B_2D00_471F_BC3F_F65C1FCF40E1__INCLUDED_)
#define AFX_RICHEDITCTRLEX_H__5C82606B_2D00_471F_BC3F_F65C1FCF40E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichEditCtrlEx.h : header file
//

//******************************************************************************
// Owner:   Andrew Forget (AndrewFo,x3438) 
//------------------------------------------------------------------------------
//
// @class   _AFX_RICHEDITEX_STATE |
//          This is the rich edit control extended state class.
//
// @version
//------------------------------------------------------------------------------
//.Version: Date:       Author:     Comments:
//.-------- ----------- ----------- --------------------------------------------
//.  1.0    01/18/99    AndrewFo    New code.
//******************************************************************************

class _AFX_RICHEDITEX_STATE
{
	public:

			 _AFX_RICHEDITEX_STATE() ;
	virtual ~_AFX_RICHEDITEX_STATE() ;

	public:

	HINSTANCE m_hInstRichEdit20 ;
} ;


//******************************************************************************
//
// Function Prototype
//
//******************************************************************************

BOOL PASCAL AfxInitRichEditEx() ;


/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx window

class IcqMsg;

class CRichEditCtrlEx : public CRichEditCtrl
{
// Construction
public:
	CRichEditCtrlEx();

// Attributes
public:

// Operations
public:
	virtual BOOL AutoURLDetect( BOOL in_fEnable ) ;

	void setText(LPCTSTR text);
	CString getText(CString &text);
	void insertText(LPCTSTR text);
	void insertEmotion(int i);
	void insertMsg(IcqMsg &msg);
	void scrollToBottom();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRichEditCtrlEx();

	virtual BOOL Create( DWORD in_dwStyle, const RECT& in_rcRect, CWnd* in_pParentWnd, UINT in_nID ) ;

protected:
	HBITMAP GetImage(CImageList& list, int num);

	static int findEmotion(const char *str);
	static LPCTSTR emotions[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CRichEditCtrlEx)
	//}}AFX_MSG
	afx_msg void OnLink(NMHDR *pNMHDR, LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITCTRLEX_H__5C82606B_2D00_471F_BC3F_F65C1FCF40E1__INCLUDED_)
