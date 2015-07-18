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

#if !defined(AFX_GOBANGDLG_H__49394F21_4D53_48A8_841E_BE9BF102BF69__INCLUDED_)
#define AFX_GOBANGDLG_H__49394F21_4D53_48A8_841E_BE9BF102BF69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GobangDlg.h : header file
//

#include "game.h"

/////////////////////////////////////////////////////////////////////////////
// CGobangDlg dialog

class CGobangDlg : public CDialog, public GobangGame
{
// Construction
public:
	CGobangDlg(Group *g, CWnd* pParent = NULL);   // standard constructor
	~CGobangDlg();

	virtual void destroy();
	virtual void onStart();
	virtual void onMemberExited(int i);

// Dialog Data
	//{{AFX_DATA(CGobangDlg)
	enum { IDD = IDD_GOBANG };
	CStatic	m_chessman[2];
	CStatic	m_faceLabel[2];
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGobangDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	virtual void OnCancel();

private:
	virtual void onMove(int row, int col, int res);

	void updateInfo();
	void drawChessman(CDC *pDC, int row, int col);
	void drawMark(CDC *pDC);
	void setStatus(UINT id);

	CBitmap boardBitmap;
	CImageList chessmanImageList;
	HICON chessmanIcon[2];

	int lastRow, lastCol;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGobangDlg)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOBANGDLG_H__49394F21_4D53_48A8_841E_BE9BF102BF69__INCLUDED_)
