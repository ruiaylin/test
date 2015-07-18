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

#if !defined(AFX_MSGEDIT_H__1CC8BFB9_160C_4416_AA5A_6734F98E5FC9__INCLUDED_)
#define AFX_MSGEDIT_H__1CC8BFB9_160C_4416_AA5A_6734F98E5FC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyRichEditCtrl.h : header file
//

#include "RichEditCtrlEx.h"

class CFormatBar;
class MsgFormat;

/////////////////////////////////////////////////////////////////////////////
// CMsgEdit window

class CMsgEdit : public CRichEditCtrlEx
{
// Construction
public:
	CMsgEdit();

// Attributes
public:
	void setToolBar(CFormatBar *fb) {
		formatBar = fb;
	}

// Operations
public:
	void getMsgFormat(MsgFormat &f);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMsgEdit();

private:
	CFormatBar *formatBar;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgEdit)
	afx_msg void OnCharBold();
	afx_msg void OnCharItalic();
	afx_msg void OnCharUnderline();
	afx_msg void OnCharColor();
	afx_msg void OnCharEmotion();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnViewFont();
	//}}AFX_MSG
	afx_msg void OnGetCharFormat(NMHDR* pNMHDR, LRESULT* pRes);
	afx_msg void OnSetCharFormat(NMHDR* pNMHDR, LRESULT* pRes);
	afx_msg void OnBarReturn(NMHDR*, LRESULT* );
	afx_msg void OnColorPick(UINT nID);
	afx_msg void OnEmotionPick(UINT nID);
	afx_msg void OnChange();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGEDIT_H__1CC8BFB9_160C_4416_AA5A_6734F98E5FC9__INCLUDED_)
