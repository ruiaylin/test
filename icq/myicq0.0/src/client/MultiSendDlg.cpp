// MultiSendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "MultiSendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiSendDlg dialog


CMultiSendDlg::CMultiSendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiSendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMultiSendDlg)
	m_radio = 0;
	//}}AFX_DATA_INIT
	hIcon=AfxGetApp()->LoadIcon(IDR_MESSAGE1);
}


void CMultiSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultiSendDlg)
	DDX_Control(pDX, IDC_EDITMSG, m_EditMsg);
	DDX_Control(pDX, IDC_EDITNAME, m_EditName);
	DDX_Control(pDX, IDC_EDITID, m_EditId);
	DDX_Radio(pDX, IDC_RADIO1, m_radio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMultiSendDlg, CDialog)
	//{{AFX_MSG_MAP(CMultiSendDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiSendDlg message handlers

BOOL CMultiSendDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);

	m_EditMsg.LimitText(LimitMaxMsgLength);
	m_EditId.EnableWindow(FALSE);
	m_EditName.SetFocus();
	return FALSE; 
}

void CMultiSendDlg::OnOK() 
{
	if(GetApp()->m_bOnlineState==0)
	{
		CString str;
		str.LoadString(IDS_NOT_LOGIN_SERVER);
		MessageBox(str);
		return;
	}
	if(m_EditMsg.GetWindowTextLength()<1)
	{
		CString str;
		str.LoadString(IDS_CANNOT_SEND_NULL);
		MessageBox(str);
		return;
	}

	UpdateData();

	CArray<DWORD,DWORD> arID;
	if(m_radio==0)
	{
		CArray<CString,CString> arStrName;
		char rtemp[100],sztemp[20];
		if(m_EditName.GetWindowTextLength()<1)
		{
			CString str;
			str.LoadString(IDS_FILL_ID_ERROR);
			MessageBox(str);
			m_EditName.SetFocus();
			return;
		}
		if(GetApp()->m_aUserInfo.GetSize()==1)
			return;
		m_EditName.GetWindowText(rtemp,99);
			
		for(int i=0,j=0;rtemp[i];i++)
		{		
			if(rtemp[i]==',')
			{
				sztemp[j]='\0';
				arStrName.Add(sztemp);
				j=0;
			}
			else
				sztemp[j++]=rtemp[i];
		}
		sztemp[j]='\0';
		arStrName.Add(sztemp);

		for(int k=0;k<arStrName.GetSize();k++)
		{
			for(i=1;i<GetApp()->m_aUserInfo.GetSize();i++)
			{
				if(GetApp()->m_aUserInfo.GetAt(i)->Name==arStrName.GetAt(k))
				{
					arID.Add(GetApp()->m_aUserInfo.GetAt(i)->Id);
					break;
				}
			}
		}
	}
	else if(m_radio==1)
	{
		// user id		
		char rtemp[100],sztemp[20];
		if(m_EditId.GetWindowTextLength()<1)
		{
			CString str;
			str.LoadString(IDS_FILL_ID_ERROR);
			MessageBox(str);
			m_EditId.SetFocus();
			return;
		}		
		m_EditId.GetWindowText(rtemp,99);
			
		for(int i=0,j=0;rtemp[i];i++)
		{		
			if(rtemp[i]==',')
			{
				sztemp[j]='\0';
				arID.Add(atol(sztemp));
				j=0;
			}
			else
				sztemp[j++]=rtemp[i];
		}
		sztemp[j]='\0';
		arID.Add(atol(sztemp));
	}
	else
	{
		return;
	}

	if(arID.GetSize()<1)
		return;

	CMsg2 msg;
	msg.index=MULTI_SEND_MSG;
	msg.MyId=GetApp()->m_uCurrentUserID;	

	for(int i=0;i<arID.GetSize();i++)
	{
		msg.aFriendId.Add(arID.GetAt(i));
	}
	m_EditMsg.GetWindowText(msg.Msg);
	msg.Time=CTime::GetCurrentTime();
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;
	m_bCancel=FALSE;

tryagain:
	if(GetApp()->m_Socket.SendData(&msg,m_bCancel))
	{	
		m_EditMsg.SetWindowText("");
		m_EditName.SetWindowText("");
		m_EditId.SetWindowText("");
		m_EditMsg.SetReadOnly(FALSE);
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
			GetDlgItem(IDOK)->EnableWindow();
			m_EditMsg.SetFocus();
		}		
	}
	else
	{
		m_EditMsg.SetWindowText("");
		m_EditName.SetWindowText("");
		m_EditId.SetWindowText("");
		m_EditMsg.SetReadOnly(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CMultiSendDlg::OnRadio1() 
{
	m_EditId.EnableWindow(FALSE);
	m_EditName.EnableWindow(TRUE);
}

void CMultiSendDlg::OnRadio2() 
{
	m_EditId.EnableWindow(TRUE);
	m_EditName.EnableWindow(FALSE);
}

void CMultiSendDlg::OnCancel() 
{
	m_EditMsg.SetWindowText("");
	m_EditName.SetWindowText("");
	m_EditId.SetWindowText("");
	m_bCancel=TRUE;
	CDialog::OnCancel();
}

void CMultiSendDlg::OnClose() 
{
	OnCancel();
}

BOOL CMultiSendDlg::PreTranslateMessage(MSG* pMsg) 
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
