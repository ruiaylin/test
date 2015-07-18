// GroupModePage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupModePage.h"
#include "GroupWizard.h"
#include "udpsession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupModePage property page

IMPLEMENT_DYNCREATE(CGroupModePage, CPropertyPage)

CGroupModePage::CGroupModePage() : CPropertyPage(CGroupModePage::IDD)
{
	//{{AFX_DATA_INIT(CGroupModePage)
	m_mode = 0;
	//}}AFX_DATA_INIT
}

CGroupModePage::~CGroupModePage()
{
}

int CGroupModePage::getSelectedItem()
{
	int i = -1;
	POSITION pos = m_gameList.GetFirstSelectedItemPosition();
	if (pos)
		i = m_gameList.GetNextSelectedItem(pos);

	return i;
}

void CGroupModePage::onGameListReply(GAME_INFO info[], int n)
{
	for (int i = 0; i < n; i++) {
		m_gameList.InsertItem(i, info[i].displayName.c_str());
		string name = info[i].name + ".dll";
		m_gameList.SetItemText(i, 2, name.c_str());
	}
}

void CGroupModePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupModePage)
	DDX_Control(pDX, IDC_GAME_LIST, m_gameList);
	DDX_Radio(pDX, IDC_NEW_GAME, m_mode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupModePage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupModePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupModePage message handlers

void CGroupModePage::OnCancel() 
{
	GetParent()->DestroyWindow();
}

BOOL CGroupModePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString str;
	str.LoadString(IDS_GAME);
	m_gameList.InsertColumn(0, str, LVCFMT_LEFT, 100);
	str.LoadString(IDS_NUM_PEOPLE);
	m_gameList.InsertColumn(1, str, LVCFMT_LEFT, 40);
	str.LoadString(IDS_PLUGIN);
	m_gameList.InsertColumn(2, str, LVCFMT_LEFT, 150);

	getUdpSession()->getGameList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CGroupModePage::OnWizardNext() 
{
	UpdateData();

	CGroupWizard *wiz = (CGroupWizard *) GetParent();

	if (m_mode == GROUP_MODE_CREATE || m_mode == GROUP_MODE_LIST) {
		int i = getSelectedItem();
		if (i < 0)
			return -1;

		wiz->gameType = i;
		wiz->gamePlugin = m_gameList.GetItemText(i, 2);

		if (m_mode == GROUP_MODE_CREATE)
			return IDD_GROUP_FINISH;
	}

	return CPropertyPage::OnWizardNext();
}

BOOL CGroupModePage::OnSetActive() 
{
	((CPropertySheet *) GetParent())->SetWizardButtons(PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}
