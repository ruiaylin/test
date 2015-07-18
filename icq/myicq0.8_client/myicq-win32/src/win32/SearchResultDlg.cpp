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

// SearchResultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "SearchResultDlg.h"
#include "SearchWizard.h"
#include "ViewDetailDlg.h"
#include "serversession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int CALLBACK compareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SEARCH_RESULT *p1 = (SEARCH_RESULT *) lParam1;
	SEARCH_RESULT *p2 = (SEARCH_RESULT *) lParam2;
	switch (lParamSort) {
	case 0:		// uin
		if (p1->uin > p2->uin)
			return 1;
		else if (p1->uin < p2->uin)
			return -1;
		return 0;

	case 1:		// nick
		return p1->nick.compare(p2->nick);

	case 2:		// province
		return p1->province.compare(p2->province);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CSearchResultDlg property page

IMPLEMENT_DYNCREATE(CSearchResultDlg, CPropertyPage)

CSearchResultDlg::CSearchResultDlg() : CPropertyPage(CSearchResultDlg::IDD)
{
	//{{AFX_DATA_INIT(CSearchResultDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pageCount = 0;
}

CSearchResultDlg::~CSearchResultDlg()
{
	deleteResults();
}


SEARCH_RESULT *CSearchResultDlg::getResult()
{
	POSITION pos = m_ctlResult.GetFirstSelectedItemPosition();
	if (pos) {
		int sel = m_ctlResult.GetNextSelectedItem(pos);
		return (SEARCH_RESULT *) m_ctlResult.GetItemData(sel);
	}
	return NULL;
}

void CSearchResultDlg::enableControls(BOOL enable)
{
	GetDlgItem(IDC_SEARCH_PREV)->EnableWindow(enable && curPage > 0);
	GetDlgItem(IDC_SEARCH_NEXT)->EnableWindow(enable && curPage < MAX_PAGES - 1);
	GetDlgItem(IDC_SEARCH_DETAIL)->EnableWindow(enable);
	GetDlgItem(IDC_SEARCH_ALL)->EnableWindow(enable);
}

void CSearchResultDlg::displayPage(int page, BOOL showStatus)
{
	if (page < 0 || page >= pageCount)
		return;

	if (showStatus) {
		CString str;
		str.Format(IDS_SEARCH_PAGE_NO, page + 1);
		SetDlgItemText(IDC_STATUS, str);
	}

	PtrList *l = pages[page];
	PtrList::iterator it;
	for (it = l->begin(); it != l->end(); ++it) {
		SEARCH_RESULT *p = (SEARCH_RESULT *) *it;

		int image = p->face;
		if (!p->online)
			image += getApp()->nrFaces;
		CString str;
		str.Format("%lu", p->uin);
		int i = m_ctlResult.InsertItem(m_ctlResult.GetItemCount(), str, image);
		m_ctlResult.SetItemData(i, (DWORD) p);
		m_ctlResult.SetItemText(i, 1, p->nick.c_str());
		m_ctlResult.SetItemText(i, 2, p->province.c_str());
	}
}

void CSearchResultDlg::deleteResults()
{
	for (int i = 0; i < pageCount; i++) {
		PtrList *l = pages[i];
		PtrList::iterator it;
		for (it = l->begin(); it != l->end(); ++it)
			delete (SEARCH_RESULT *) *it;
		l->clear();
	}
	curPage = -1;
	pageCount = 0;
	startUIN = 0;
}

void CSearchResultDlg::onSendError(uint32 seq)
{
	myMessageBox(IDS_TIMEOUT, IDS_FAILED, this);
}

void CSearchResultDlg::onUserFound(PtrList *l)
{
	if (!l->empty()) {
		pages[pageCount++] = l;
		displayPage(++curPage);
		startUIN = ((SEARCH_RESULT *) l->back())->uin;
	}

	enableControls();
}


void CSearchResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchResultDlg)
	DDX_Control(pDX, IDC_SEARCH_RESULT, m_ctlResult);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchResultDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSearchResultDlg)
	ON_BN_CLICKED(IDC_SEARCH_ALL, OnSearchAll)
	ON_BN_CLICKED(IDC_SEARCH_NEXT, OnSearchNext)
	ON_BN_CLICKED(IDC_SEARCH_DETAIL, OnSearchDetail)
	ON_BN_CLICKED(IDC_SEARCH_PREV, OnSearchPrev)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_SEARCH_RESULT, OnColumnClick)
	ON_NOTIFY(NM_DBLCLK, IDC_SEARCH_RESULT, OnDblclkSearchResult)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchResultDlg message handlers

BOOL CSearchResultDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_ctlResult.SetExtendedStyle(m_ctlResult.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ctlResult.SetImageList(&getApp()->smallImageList, LVSIL_SMALL);
	
	CString str;
	str.LoadString(IDS_UIN);
	m_ctlResult.InsertColumn(0, str, LVCFMT_LEFT, 90);
	str.LoadString(IDS_NICK);
	m_ctlResult.InsertColumn(1, str, LVCFMT_CENTER, 90);
	str.LoadString(IDS_FROM_WHERE);
	m_ctlResult.InsertColumn(2, str, LVCFMT_CENTER, 120);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSearchResultDlg::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	m_ctlResult.DeleteAllItems();
	deleteResults();

	CString str;
	str.Format(IDS_TITLE_SEARCH_RESULT, serverSession()->sessionCount);
	SetWindowText(str);

	OnSearchNext();

	return CPropertyPage::OnSetActive();
}

void CSearchResultDlg::OnSearchAll() 
{
	CString str;
	str.Format(IDS_SEARCH_RESULT_ALL, pageCount);
	SetDlgItemText(IDC_STATUS, str);
	
	m_ctlResult.DeleteAllItems();
	for (int i = 0; i < pageCount; i++)
		displayPage(i, FALSE);
}

void CSearchResultDlg::OnSearchPrev()
{
	if (--curPage <= 0)
		GetDlgItem(IDC_SEARCH_PREV)->EnableWindow(FALSE);
	m_ctlResult.DeleteAllItems();
	displayPage(curPage);
}

void CSearchResultDlg::OnSearchNext() 
{
	m_ctlResult.DeleteAllItems();

	CString str;
	str.LoadString(IDS_SEARCHING_CONTACT);
	SetDlgItemText(IDC_STATUS, str);

	if (curPage < pageCount - 1) {
		if (curPage == 0)
			GetDlgItem(IDC_SEARCH_PREV)->EnableWindow();
		displayPage(++curPage);
		return;
	}
	
	CSearchWizard *wiz = (CSearchWizard *) GetParent();
	int mode = wiz->modeDlg.m_mode;
	if (mode == SEARCH_MODE_ONLINE)
		wiz->seq = serverSession()->searchRandom();
	else if (mode == SEARCH_MODE_CUSTOM) {
		uint32 uin = wiz->customDlg.m_uin;
		const char *nick = wiz->customDlg.m_nick;
		const char *email = wiz->customDlg.m_email;
		wiz->seq = serverSession()->searchCustom(uin, nick, email, startUIN);
	}

	enableControls(FALSE);
}

void CSearchResultDlg::OnSearchDetail() 
{
	SEARCH_RESULT *result = getResult();
	if (result) {
		CViewDetailDlg *win = new CViewDetailDlg(result->uin);
		win->Create(this);
	}
}

LRESULT CSearchResultDlg::OnWizardBack() 
{
	CSearchWizard *wiz = (CSearchWizard *) GetParent();
	if (wiz->modeDlg.m_mode == SEARCH_MODE_ONLINE)
		return IDD_SEARCH_MODE;
	else
		return CPropertyPage::OnWizardBack();
}

void CSearchResultDlg::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_ctlResult.SortItems(compareFunc, (LPARAM) pNMListView->iSubItem);
	*pResult = 0;
}

LRESULT CSearchResultDlg::OnWizardNext() 
{
	SEARCH_RESULT *result = getResult();
	if (!result) {
		myMessageBox(IDS_SELECT_CONTACT, IDS_ERROR, this);
		return -1;
	}
	if (result->uin == icqLink->myInfo.uin) {
		myMessageBox(IDS_ERROR_ADD_YOURSELF, IDS_ERROR, this);
		return -1;
	}
	return CPropertyPage::OnWizardNext();
}

void CSearchResultDlg::OnDblclkSearchResult(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnSearchDetail();
	*pResult = 0;
}
