// SendBroadcastDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "SendBroadcastDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendBroadcastDlg dialog


CSendBroadcastDlg::CSendBroadcastDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendBroadcastDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendBroadcastDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	hIcon=AfxGetApp()->LoadIcon(IDR_MESSAGE1);
}



void CSendBroadcastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendBroadcastDlg)
	DDX_Control(pDX, IDC_PWD, m_EditPwd);
	DDX_Control(pDX, IDC_MSG, m_EditMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendBroadcastDlg, CDialog)
	//{{AFX_MSG_MAP(CSendBroadcastDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendBroadcastDlg message handlers

BOOL CSendBroadcastDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);	
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);

	m_EditMsg.LimitText(LimitMaxMsgLength);
	m_EditMsg.SetFocus();
	
	return FALSE; 
}

void CSendBroadcastDlg::OnOK() 
{
	if(GetApp()->m_bOnlineState==0)
	{
		CString str;
		str.LoadString(IDS_NOT_LOGIN_SERVER);
		MessageBox(str);
		return;
	}
	
	if(m_EditPwd.GetWindowTextLength()<1)
	{
		CString str;
		str.LoadString(IDS_PWD_NOT_NULL);
		MessageBox(str);
		m_EditPwd.SetFocus();
		return;
	}
	if(m_EditMsg.GetWindowTextLength()<1)
	{
		CString str;
		str.LoadString(IDS_CANNOT_SEND_NULL);
		MessageBox(str);
		m_EditMsg.SetFocus();
		return;
	}
	CMsg4 msg;
	msg.index=SEND_BROADCAST;
	msg.nPort=GetApp()->m_uServerPort;
	msg.tarIP=GetApp()->m_uServerIP;
	msg.MyId=GetApp()->m_uCurrentUserID;
	m_EditPwd.GetWindowText(msg.BroadcastPwd);
	m_EditMsg.GetWindowText(msg.Msg);
	
	m_bCancel=FALSE;
tryagain:
	if(GetApp()->m_Socket.SendData(&msg,m_bCancel))
	{
		m_EditMsg.SetWindowText("");
		m_EditPwd.SetWindowText("");
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
			m_EditMsg.SetReadOnly(FALSE);
			m_EditMsg.SetWindowText("");
			m_EditPwd.SetWindowText("");		
			GetDlgItem(IDOK)->EnableWindow();
			m_EditMsg.SetFocus();
		}		
	}
	else
	{
		m_EditMsg.SetWindowText("");
		m_EditPwd.SetWindowText("");
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CSendBroadcastDlg::OnCancel() 
{
	m_EditPwd.SetWindowText("");	
	m_EditMsg.SetWindowText("");
	m_bCancel=TRUE;
	CDialog::OnCancel();
}

void CSendBroadcastDlg::OnClose() 
{
	OnCancel();
}

BOOL CSendBroadcastDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN&&GetKeyState(VK_CONTROL)&0x80)
		{
			OnOK();
			return 1;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
