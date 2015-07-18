// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "LoginDlg.h"
#include "msg.h"
#include "setupserverdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog


CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_LoginType = FALSE;
	//}}AFX_DATA_INIT
	lresult=0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Control(pDX, ID_USER_PWD, m_EditPwd);
	DDX_Control(pDX, ID_UID_LIST, m_IdList);
	DDX_Check(pDX, ID_ONHIDE_LOGIN, m_LoginType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	ON_WM_TIMER()	
	ON_BN_CLICKED(ID_USER_REGISTR, OnUserRegistr)
	ON_WM_PAINT()
	ON_MESSAGE(WM_RECIEVE_MSG,ProcRecv)
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

void CLoginDlg::OnOK() 
{
	CString	selected;
	int sel=m_IdList.GetCurSel();
	m_IdList.GetLBText(sel,selected);
	selected+="\\";
	uCurID=atol(selected);
		
	CFile file;
	if(file.Open(selected+USERFILE,CFile::modeRead))
	{
		int offsetLength=UserFileReservedLength;	
		char pass[MaxPasswordLength];
		file.Seek(offsetLength,CFile::begin);
		UCHAR ch;
		file.Read(&ch,sizeof(UCHAR));
		file.Seek(2,CFile::current);
		file.Read(pass,ch);
		file.Close();
		pass[ch]='\0';
		CString CurPwd;
		m_EditPwd.GetWindowText(CurPwd);
		if(!CurPwd.Compare(pass))
		{
			pwd=CurPwd;
			CFile f1;
			if(f1.Open(selected+USERSETUP,CFile::modeRead))
			{				
				f1.Read((char*)&uServerIP,sizeof(DWORD));
				f1.Read((char*)&uPort,sizeof(DWORD));
				f1.Close();
				CDialog::OnOK();
				return;
			}
		}			
	}
	
	CString str;
	str.LoadString(IDS_ASK_CHECK_PWD_ONSERVER);
	if(MessageBox(str,NULL,MB_YESNO|MB_DEFBUTTON2)==IDNO)
	{
		m_EditPwd.SetWindowText("");
		m_EditPwd.SetFocus();
		return;
	}

	if(!file.Open(selected+USERSETUP,CFile::modeRead))
	{
		CSetupServerDlg dlg;
		dlg.m_strServerIP=DEFAULT_SERVER_IP;
		dlg.m_port=DEFAULT_SERVER_PORT;
		if(dlg.DoModal()==IDCANCEL)
		{
			CDialog::OnCancel();
			return;
		}
		uServerIP=dlg.uServerIP;
		uPort=dlg.uPort;
		file.Open(selected+USERSETUP,CFile::modeCreate|CFile::modeWrite);
		file.Write(&uServerIP,sizeof(DWORD));
		file.Write(&uPort,sizeof(DWORD));
		file.Close();
	}
	else
	{
		file.Read(&uServerIP,sizeof(DWORD));
		file.Read(&uPort,sizeof(DWORD));
		file.Close();	
	}
	CMsgModifyPwd msg;
	msg.index=HAVE_ID_LOGIN;
	msg.MyId=uCurID;
	m_EditPwd.GetWindowText(msg.OldPwd);
	msg.tarIP=uServerIP;
	msg.nPort=uPort;
	pwd=msg.OldPwd;
	CClientApp* pApp=(CClientApp*)AfxGetApp();
	pApp->m_Socket.SendDataDirect(&msg);
	SetTimer(1,400,NULL);
	BeginWaitCursor();
}

void CLoginDlg::SetUserID(CArray<int,int> &aID)
{
	CString buf;
	for(int i=aID.GetSize()-1;i>=0;i--)
	{
		buf.Format("%d",aID.GetAt(i));
		m_aID.Add(buf);
	}	
}

void CLoginDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
	{
		EndWaitCursor();
		KillTimer(1);
		CString str;
		str.LoadString(IDS_SERVER_NOT_RES);
		MessageBox(str);
		CDialog::OnCancel();	
	}	
}

LRESULT CLoginDlg::ProcRecv(WPARAM wParam,LPARAM lParam)
{
	CData* pData=(CData*)wParam;
	if(pData->index==RE_LOGIN_INFO)
	{
		CMsg3 msg;
		CopyDataBuf(&msg,pData);
		if(msg.LoadFromBuf())
		{
			KillTimer(1);
			EndWaitCursor();
			if(msg.Value==2)
			{				
				CDialog::OnOK();				
			}
			else if(msg.Value==1)
			{				
				CString str;
				str.LoadString(IDS_PWD_ERROR);
				MessageBox(str);
				CDialog::OnCancel();				
			}
			else
			{
				CString str;
				str.LoadString(IDS_ID_NOT_EXIST);
				MessageBox(str);
				CDialog::OnCancel();				
			}
		}
	}
	delete pData;
	return 0;
}

void CLoginDlg::OnUserRegistr() 
{
	lresult=1;
	CDialog::OnOK();
}

BOOL CLoginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);		
	SetIcon(m_hIcon, FALSE);
	
	for(int i=0;i<m_aID.GetSize();i++)
		m_IdList.AddString(m_aID.GetAt(i));	
	m_IdList.SetCurSel(0);
	
	m_EditPwd.SetFocus();
	return FALSE;  
}

void CLoginDlg::OnPaint() 
{	
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	} 
}

HCURSOR CLoginDlg::OnQueryDragIcon() 
{
	return (HCURSOR) m_hIcon;
}
