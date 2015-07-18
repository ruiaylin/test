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

#if !defined(AFX_CAPTIONBTNDLG_H__1C564C2E_F896_4AE9_92B4_864998452353__INCLUDED_)
#define AFX_CAPTIONBTNDLG_H__1C564C2E_F896_4AE9_92B4_864998452353__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaptionBtnDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCaptionBtnDlg dialog

class CCaptionBtnDlg : public CDialog
{
// Construction
public:
	CCaptionBtnDlg(int buttons, UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCaptionBtnDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaptionBtnDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void drawButtons();
	void drawButton(int button, BOOL pressed);
	int hitTest(CPoint &pt);

	int nrButtons;
	int pressedButton;
	int inButton;

// Implementation
protected:
	virtual void drawCaptionBtn(CDC *pDC, CRect &rc, int button, BOOL pressed) = 0;
	virtual void onCaptionBtnClicked(int button) = 0;

	// Generated message map functions
	//{{AFX_MSG(CCaptionBtnDlg)
	afx_msg void OnNcPaint();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	//}}AFX_MSG
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTIONBTNDLG_H__1C564C2E_F896_4AE9_92B4_864998452353__INCLUDED_)
