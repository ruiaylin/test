// ServerView.h : interface of the CServerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERVIEW_H__C318997F_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_)
#define AFX_SERVERVIEW_H__C318997F_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServerDoc;

class CServerView : public CEditView
{
protected: // create from serialization only
	CServerView();
	DECLARE_DYNCREATE(CServerView)

// Attributes
public:
	CServerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowMessage(CString str);
	virtual ~CServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CCriticalSection m_criticalShowMess;
// Generated message map functions
protected:
	//{{AFX_MSG(CServerView)
	afx_msg void OnEditCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ServerView.cpp
inline CServerDoc* CServerView::GetDocument()
   { return (CServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERVIEW_H__C318997F_4DB1_11D5_B121_A3A17EBFEA00__INCLUDED_)
