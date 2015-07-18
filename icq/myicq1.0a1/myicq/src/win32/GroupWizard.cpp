// GroupWizard.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "GroupWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupWizard

IMPLEMENT_DYNAMIC(CGroupWizard, CPropertySheet)

CGroupWizard::CGroupWizard() : IcqWindow(WIN_GROUP_WIZARD)
{
	gameType = -1;
	
	SetWizardMode();
	m_psh.dwFlags |= PSH_USEHICON;
	m_psh.hIcon = getApp()->m_hIcon;

	AddPage(&modePage);
	AddPage(&listPage);
	AddPage(&finishPage);

	Create();
}

CGroupWizard::~CGroupWizard()
{
}


BEGIN_MESSAGE_MAP(CGroupWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CGroupWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupWizard message handlers

void CGroupWizard::PostNcDestroy() 
{
	delete this;
}

BOOL CGroupWizard::Create(CWnd *parent) 
{
	DWORD dwStyle = WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE | WS_MINIMIZEBOX;
	DWORD dwExStyle = WS_EX_DLGMODALFRAME | WS_EX_APPWINDOW;
	return CPropertySheet::Create(parent, dwStyle, dwExStyle);
}
