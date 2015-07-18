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

// GroupSearchResultPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupSearchResultPage.h"
#include "GroupSearchWizard.h"
#include "icqlink.h"
#include "udpsession.h"
#include "groupplugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchResultPage property page

IMPLEMENT_DYNCREATE(CGroupSearchResultPage, CPropertyPage)

CGroupSearchResultPage::CGroupSearchResultPage() : CPropertyPage(CGroupSearchResultPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupSearchResultPage)
	m_passwd = _T("");
	//}}AFX_DATA_INIT
}

CGroupSearchResultPage::~CGroupSearchResultPage()
{
}

void CGroupSearchResultPage::deleteAllItems()
{
	int n = m_resultList.GetItemCount();
	for (int i = 0; i < n; i++) {
		GROUP_INFO *p = (GROUP_INFO *) m_resultList.GetItemData(i);
		delete p;
	}

	m_resultList.DeleteAllItems();
}

GROUP_INFO *CGroupSearchResultPage::getResult()
{
	GROUP_INFO *info = NULL;
	POSITION pos = m_resultList.GetFirstSelectedItemPosition();
	if (pos) {
		int i = m_resultList.GetNextSelectedItem(pos);
		info = (GROUP_INFO *) m_resultList.GetItemData(i);
	}
	return info;
}

void CGroupSearchResultPage::addResult(GROUP_INFO *info)
{
	int n = m_resultList.GetItemCount();

	CString str;
	str.Format("%lu", info->id);
	m_resultList.InsertItem(n, str);
	m_resultList.SetItemText(n, 1, info->name.c_str());
	str.Format("%d", (int) info->num);
	m_resultList.SetItemText(n, 2, str);

	m_resultList.SetItemData(n, (DWORD) info);
}

void CGroupSearchResultPage::onSendError()
{
	CString str;
	str.LoadString(IDS_TIMEOUT);
	MessageBox(str);

	GetDlgItem(IDC_REFRESH)->EnableWindow();
}

void CGroupSearchResultPage::onGroupListReply(PtrList &l)
{
	CGroupSearchWizard *wiz = (CGroupSearchWizard *) GetParent();
	int type = wiz->typePage.getGroupType();

	PtrList::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		GROUP_INFO *p = (GROUP_INFO *) *it;
		p->type = type;
		addResult(p);
	}

	GetDlgItem(IDC_REFRESH)->EnableWindow();
}

void CGroupSearchResultPage::onSearchGroupReply(GROUP_INFO *info)
{
	GetDlgItem(IDC_REFRESH)->EnableWindow();

	if (info)
		addResult(info);
}

void CGroupSearchResultPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupSearchResultPage)
	DDX_Control(pDX, IDC_SEARCH_RESULT, m_resultList);
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupSearchResultPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupSearchResultPage)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupSearchResultPage message handlers

BOOL CGroupSearchResultPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_resultList.SetExtendedStyle(m_resultList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	CString str;
	str.LoadString(IDS_GROUP_ID);
	m_resultList.InsertColumn(0, str, LVCFMT_LEFT, 60);
	str.LoadString(IDS_GROUP_NAME);
	m_resultList.InsertColumn(1, str, LVCFMT_CENTER, 90);
	str.LoadString(IDS_NUM_PEOPLE);
	m_resultList.InsertColumn(2, str, LVCFMT_CENTER, 40);

	OnRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGroupSearchResultPage::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}

LRESULT CGroupSearchResultPage::OnWizardBack() 
{
	CGroupSearchWizard *wiz = (CGroupSearchWizard *) GetParent();

	if (wiz->modePage.m_mode == 1)
		return IDD_GROUP_SEARCH_MODE;

	return CPropertyPage::OnWizardBack();
}

LRESULT CGroupSearchResultPage::OnWizardNext() 
{
	GROUP_INFO *info = getResult();
	if (!info)
		return -1;

	CGroupSearchWizard *wiz = (CGroupSearchWizard *) GetParent();
	GROUP_TYPE_INFO *type = icqLink->getGroupTypeInfo(info->type);
	if (!GroupPlugin::get(type->name.c_str()))
		return -1;

	return CPropertyPage::OnWizardNext();
}

void CGroupSearchResultPage::OnRefresh() 
{
	deleteAllItems();

	CGroupSearchWizard *wiz = (CGroupSearchWizard *) GetParent();

	if (wiz->modePage.m_mode == 0) {
		int type = wiz->typePage.getGroupType();
		wiz->seq = getUdpSession()->getGroupList(type);
	} else {
		uint32 id = wiz->modePage.m_groupID;
		wiz->seq = getUdpSession()->searchGroup(id);
	}

	GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
}

void CGroupSearchResultPage::OnDestroy() 
{
	deleteAllItems();

	CPropertyPage::OnDestroy();	
}
