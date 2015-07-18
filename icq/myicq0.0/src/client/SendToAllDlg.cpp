// SendToAllDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "SendToAllDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendToAllDlg dialog


CSendToAllDlg::CSendToAllDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendToAllDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendToAllDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	hIcon=AfxGetApp()->LoadIcon(IDR_MESSAGE1);
}


void CSendToAllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendToAllDlg)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendToAllDlg, CDialog)
	//{{AFX_MSG_MAP(CSendToAllDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendToAllDlg message handlers

void CSendToAllDlg::OnOK() 
{
	if(GetApp()->m_bOnlineState==0)
	{
		CString str;
		str.LoadString(IDS_NOT_LOGIN_SERVER);
		MessageBox(str);
		return;
	}
	if(GetApp()->m_aUserInfo.GetSize()==1)
		return;

	CMsg2 msg;
	msg.index=MULTI_SEND_MSG;
	msg.MyId=GetApp()->m_uCurrentUserID;
	
	for(int i=1;i<GetApp()->m_aUserInfo.GetSize();i++)
	{
		msg.aFriendId.Add(GetApp()->m_aUserInfo.GetAt(i)->Id);
	}
	m_Edit.GetWindowText(msg.Msg);
	msg.Time=CTime::GetCurrentTime();
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;

	m_bCancel=FALSE;
tryagain:
	if(GetApp()->m_Socket.SendData(&msg,m_bCancel))
	{
		m_Edit.SetWindowText("");
		m_Edit.SetReadOnly(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);		
		CDialog::OnOK();
	}
	else if(!m_bCancel)
	{		
		if(AfxMessageBox(IDS_ASK_RETRY,MB_YESNO)==IDYES)
		{
			goto tryagain;
		}
		else
		{				
			m_Edit.SetReadOnly(FALSE);
			GetDlgItem(IDOK)->EnableWindow();
			m_Edit.SetFocus();
		}
	}
	else
	{
		m_Edit.SetWindowText("");
		m_Edit.SetReadOnly(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

BOOL CSendToAllDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);

	m_Edit.LimitText(LimitMaxMsgLength);
	m_Edit.SetFocus();
 	
	return FALSE;
}

void CSendToAllDlg::OnClose() 
{
	OnCancel();
}

void CSendToAllDlg::OnCancel() 
{
	m_Edit.SetWindowText("");
	m_bCancel=TRUE;
	CDialog::OnCancel();
}

BOOL CSendToAllDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN&&(GetKeyState(VK_CONTROL)&0x80||GetKeyState(VK_RCONTROL)&0x80))
		{
			OnOK();
			return 1;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
