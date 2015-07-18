// SetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "server.h"
#include "SetupDlg.h"
#include <odbcinst.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog


CSetupDlg::CSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDlg)
	m_strIP = _T("");
	m_strPort = _T("");
	m_strPwd = _T("");
	m_uSendNum = 0;
	m_strUid = _T("");
	m_strDS = _T("");
	m_strBroadcastPwd = _T("");
	//}}AFX_DATA_INIT
}


void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDlg)
	DDX_Control(pDX, IDC_LISTPOST, m_ListPost);
	DDX_Text(pDX, IDC_IP, m_strIP);
	DDX_Text(pDX, IDC_PORT, m_strPort);
	DDX_Text(pDX, IDC_PWD, m_strPwd);
	DDX_Text(pDX, IDC_SENDPORT, m_uSendNum);
	DDV_MinMaxUInt(pDX, m_uSendNum, 1, 10);
	DDX_Text(pDX, IDC_UID, m_strUid);
	DDX_Text(pDX, IDC_DATASOURCE, m_strDS);
	DDX_Text(pDX, IDC_BROADCASTPWD, m_strBroadcastPwd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DEFAULT_SETUP, OnDefaultSetup)
	ON_BN_CLICKED(IDC_CONFIG_DS, OnConfigDs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg message handlers

void CSetupDlg::OnAdd() 
{
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_PORT);
	CString str;
	pEdit->GetWindowText(str);

	if(pEdit->GetWindowTextLength()==0)
	{
		pEdit->SetWindowText("");
		pEdit->SetFocus();
		return;
	}
	for(int i=0;i<str.GetLength();i++)
		if(str.GetAt(i)<'0'||str.GetAt(i)>'9')
	{
		pEdit->SetWindowText("");
		pEdit->SetFocus();
		return;
	}
	
	m_ListPost.AddString(str);
	pEdit->SetWindowText("");
}

void CSetupDlg::OnDelete() 
{
	int mIndex=0;
	mIndex=m_ListPost.GetCurSel();
	if(mIndex==LB_ERR)
		return;
	m_ListPost.DeleteString(mIndex);
}

void CSetupDlg::OnOK() 
{
	UpdateData();	
	CString strPort,st;		
	for(int i=0;i<m_ListPost.GetCount();i++)
	{
		m_ListPost.GetText(i,st);
		if(i!=0)
			st=","+st;
		strPort+=st;
	}
	CString tt;
	tt.Format("%d",m_uSendNum);
	WritePrivateProfileString("数据库", "DSN",m_strDS,SETUPFILE);
	WritePrivateProfileString("数据库", "UID",m_strUid,SETUPFILE);
	WritePrivateProfileString("数据库", "PWD",m_strPwd,SETUPFILE);
	WritePrivateProfileString("网络设置","发送端口数",tt,SETUPFILE);
	WritePrivateProfileString("网络设置","发送广播密码",m_strBroadcastPwd,SETUPFILE);	
	WritePrivateProfileString("网络设置","接受端口",strPort,SETUPFILE);

	MessageBox("修改的设置，只有在下次启动时，才能生效!");
	CDialog::OnOK();
}

BOOL CSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for(int i=0;i<m_aStr.GetSize();i++)
		m_ListPost.AddString(m_aStr.GetAt(i));
	char name[20];
	hostent * hostinfo=NULL;
	if(gethostname(name,sizeof(name)) == 0)
	{
		hostinfo = gethostbyname(name);
		if(hostinfo!= NULL)
		{
			m_strIP= inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
		}
	}
 	UpdateData(FALSE);
	return TRUE; 
}

void CSetupDlg::OnDefaultSetup() 
{
	m_ListPost.ResetContent();
	m_uSendNum=DEFAULT_SEND_NUM;
	m_strBroadcastPwd=DEFAULT_BROADCAST_PWD;
	m_strDS=DEFAULT_DSN;	
	m_strPwd=DEFAULT_DATAS_PWD;
	m_strUid=DEFAULT_UID;
	char rtemp[50],sztemp[20];
	strcpy(rtemp,DEFAULT_RECV_PORT_STR);	
	int i,j=0;
	for(i=0;rtemp[i];i++)
	{		
		if(rtemp[i]==',')
		{
			sztemp[j]='\0';
			m_ListPost.AddString(sztemp);
			j=0;
		}
		else
			sztemp[j++]=rtemp[i];
	}
	sztemp[j]='\0';
	m_ListPost.AddString(sztemp);
	UpdateData(FALSE);
}

void CSetupDlg::OnConfigDs() 
{
	SQLConfigDataSource(NULL,NULL,NULL,NULL);	
}
