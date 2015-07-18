// SetupServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "SetupServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupServerDlg dialog


CSetupServerDlg::CSetupServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupServerDlg)
	m_strServerIP = _T("");
	m_port = 0;
	//}}AFX_DATA_INIT
}


void CSetupServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupServerDlg)
	DDX_Text(pDX, IDC_EDIT1, m_strServerIP);
	DDX_Text(pDX, IDC_EDIT2, m_port);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupServerDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupServerDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupServerDlg message handlers

void CSetupServerDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	uServerIP=inet_addr(m_strServerIP);
	uPort=m_port;
	CDialog::OnOK();
}
