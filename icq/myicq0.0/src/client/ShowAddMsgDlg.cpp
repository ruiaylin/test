// ShowAddMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "ShowAddMsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowAddMsgDlg dialog


CShowAddMsgDlg::CShowAddMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowAddMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowAddMsgDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CShowAddMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowAddMsgDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowAddMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CShowAddMsgDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowAddMsgDlg message handlers
