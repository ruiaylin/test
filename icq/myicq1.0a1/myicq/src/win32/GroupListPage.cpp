// GroupListPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupListPage.h"
#include "GroupWizard.h"
#include "udpsession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupListPage property page

IMPLEMENT_DYNCREATE(CGroupListPage, CPropertyPage)

CGroupListPage::CGroupListPage() : CPropertyPage(CGroupListPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupListPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGroupListPage::~CGroupListPage()
{
}

GROUP_INFO *CGroupListPage::getGroupInfo()
{
	GROUP_INFO *info = NULL;
	POSITION pos = m_groupList.GetFirstSelectedItemPosition();
	if (pos) {
		int i = m_groupList.GetNextSelectedItem(pos);
		info = (GROUP_INFO *) m_groupList.GetItemData(i);
	}
	return info;
}

void CGroupListPage::onGroupListReply(GROUP_INFO info[], int n)
{
	m_groupList.DeleteAllItems();

	CString str;

	for (int i = 0; i < n; i++) {
		str.Format("%lu", info[i].id);
		m_groupList.InsertItem(i, str);
		str.Format("%lu", info[i].nrPlayers);
		m_groupList.SetItemText(i, 1, str);

		m_groupList.SetItemData(i, (DWORD) (info + i));
	}
}

void CGroupListPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupListPage)
	DDX_Control(pDX, IDC_GROUP_LIST, m_groupList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupListPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupListPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupListPage message handlers

BOOL CGroupListPage::OnSetActive() 
{
	CGroupWizard *wiz = (CGroupWizard *) GetParent();
	wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	if (wiz->modePage.m_mode == GROUP_MODE_LIST)
		getUdpSession()->getGroupList(wiz->gameType);

	return CPropertyPage::OnSetActive();
}

BOOL CGroupListPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString str;
	str.LoadString(IDS_GROUP_NUMBER);
	m_groupList.InsertColumn(0, str, LVCFMT_LEFT, 100);
	str.LoadString(IDS_NUM_PEOPLE);
	m_groupList.InsertColumn(1, str, LVCFMT_LEFT, 40);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
