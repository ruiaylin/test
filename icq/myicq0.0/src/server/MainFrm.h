// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__C318997B_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_)
#define AFX_MAINFRM_H__C318997B_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "trayicon.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CTrayIcon   m_TrayIcon;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateNumberOnline(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNumberAlluser(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNumberRecvReq(CCmdUI* pCmdUI);
	afx_msg void OnTest();
	afx_msg void OnShowwindow();
	//}}AFX_MSG
	afx_msg LRESULT OnTrayNotification(WPARAM wp,LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__C318997B_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_)
