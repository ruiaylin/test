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

#if !defined(AFX_SEARCHRESULTDLG_H__2CA90786_3EA8_403F_B65D_120BDFC42440__INCLUDED_)
#define AFX_SEARCHRESULTDLG_H__2CA90786_3EA8_403F_B65D_120BDFC42440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchResultDlg.h : header file
//

#include "icqtypes.h"

struct SEARCH_RESULT;

#define MAX_PAGES	100

/////////////////////////////////////////////////////////////////////////////
// CSearchResultDlg dialog

class CSearchResultDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSearchResultDlg)

// Construction
public:
	CSearchResultDlg();
	~CSearchResultDlg();

	SEARCH_RESULT *getResult();
	void onSendError(uint32 seq);
	void onUserFound(PtrList *l);

// Dialog Data
	//{{AFX_DATA(CSearchResultDlg)
	enum { IDD = IDD_SEARCH_RESULT };
	CListCtrl	m_ctlResult;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSearchResultDlg)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void enableControls(BOOL enable = TRUE);
	void displayPage(int page, BOOL showStatus = TRUE);
	void deleteResults();

	uint32 startUIN;
	int curPage;
	int pageCount;
	PtrList *pages[MAX_PAGES];
	BOOL isActiveFromNext;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSearchResultDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSearchAll();
	afx_msg void OnSearchNext();
	afx_msg void OnSearchDetail();
	afx_msg void OnSearchPrev();
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkSearchResult(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHRESULTDLG_H__2CA90786_3EA8_403F_B65D_120BDFC42440__INCLUDED_)
