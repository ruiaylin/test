// GroupFinishPage.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupFinishPage.h"
#include "GroupWizard.h"
#include "udpsession.h"
#include "icqlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupFinishPage property page

IMPLEMENT_DYNCREATE(CGroupFinishPage, CPropertyPage)

CGroupFinishPage::CGroupFinishPage() : CPropertyPage(CGroupFinishPage::IDD)
{
	//{{AFX_DATA_INIT(CGroupFinishPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGroupFinishPage::~CGroupFinishPage()
{
}

void CGroupFinishPage::onCreateGroupReply(uint32 id)
{
	CString str;
	if (id)
		str.Format(IDS_CREATE_GROUP_SUCCESS, id);
	else
		str.Format(IDS_CREATE_GROUP_FAILED);
	SetDlgItemText(IDC_STATUS, str);

	CGroupWizard *wiz = (CGroupWizard *) GetParent();
	wiz->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
}

void CGroupFinishPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupFinishPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupFinishPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupFinishPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupFinishPage message handlers

BOOL CGroupFinishPage::OnSetActive() 
{
	CGroupWizard *wiz = (CGroupWizard *) GetParent();
	wiz->SetWizardButtons(PSWIZB_DISABLEDFINISH);

	int mode = wiz->modePage.m_mode;

	if (mode == GROUP_MODE_CREATE)
		getUdpSession()->createGroup(wiz->gameType);
	else if (mode == GROUP_MODE_LIST) {
		GROUP_INFO *info = wiz->listPage.getGroupInfo();
		getUdpSession()->enterGroup(info->id);
	}

	return CPropertyPage::OnSetActive();
}

LRESULT CGroupFinishPage::OnWizardBack() 
{
	CGroupWizard *wiz = (CGroupWizard *) GetParent();
	if (wiz->modePage.m_mode == GROUP_MODE_CREATE)
		return IDD_GROUP_MODE;

	return CPropertyPage::OnWizardBack();
}

BOOL CGroupFinishPage::OnWizardFinish() 
{
	GetParent()->DestroyWindow();
	return CPropertyPage::OnWizardFinish();
}
