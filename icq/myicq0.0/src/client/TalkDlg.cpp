// TalkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "TalkDlg.h"
#include "LookDlg.h"
#include "mainfrm.h"
#include "frienddetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTalkDlg dialog


CTalkDlg::CTalkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTalkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTalkDlg)
	m_strEmail = _T("");
	m_strID = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT
	nPhotoId=0;
	pInfo=NULL;
	pState=NULL;
	hIcon=AfxGetApp()->LoadIcon(IDR_MESSAGE1);
	bClosed=TRUE;
}


void CTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTalkDlg)
	DDX_Control(pDX, IDC_SHOW_DETAIL, m_headBmp);
	DDX_Control(pDX, IDC_TALK_RECORD, m_TalkRecord);
	DDX_Control(pDX, IDC_TALK, m_TalkEdit);
	DDX_Text(pDX, IDC_EMAIL, m_strEmail);
	DDX_Text(pDX, IDC_ID, m_strID);
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTalkDlg, CDialog)
	//{{AFX_MSG_MAP(CTalkDlg)
	ON_BN_CLICKED(ID_SHOW_TALK, OnShowTalk)
	ON_BN_CLICKED(IDC_SHOW_DETAIL, OnShowDetail)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SEND_MSG,OnSendMsg)	
	ON_MESSAGE(WM_REFRESH_DATA,OnRefreshData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTalkDlg message handlers

BOOL CTalkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);

	m_TalkEdit.LimitText(LimitMaxMsgLength);
	CRect r1,r2;
	GetWindowRect(&r1);
	m_TalkRecord.GetWindowRect(&r2);
	r1.SetRect(r1.left,r1.top,r1.right,r2.top);
	MoveWindow(r1);	

	return TRUE;
}

void CTalkDlg::OnShowTalk() 
{
	static BOOL bShowRecord=TRUE;
	
	CRect r1,r2;
	GetWindowRect(&r1);
	m_TalkRecord.GetWindowRect(&r2);

	if(bShowRecord)
		r1.SetRect(r1.left,r1.top,r1.right,r2.bottom+4);	
	else
		r1.SetRect(r1.left,r1.top,r1.right,r2.top);
	bShowRecord=!bShowRecord;
	MoveWindow(r1);	
}

LRESULT CTalkDlg::OnSendMsg(WPARAM wParam,LPARAM lParam)
{
	pInfo=(UserInfo*)wParam;
	pState=(FriendState*)lParam;
	
	if(bClosed)
	{
		m_TalkEdit.SetWindowText("");
		m_strID.Format("%d",pInfo->Id);			
		nPhotoId=0;
		if(pInfo->HaveDetail)
		{
			m_strEmail=pInfo->Email;
			m_strName=pInfo->Name;
			if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
				nPhotoId=pInfo->PhotoId;			
		}
		else
		{
			m_strEmail="";
			m_strName="";			
		}
		m_headBmp.SetBitmap(GetApp()->m_paBitmap[2*nPhotoId]);
		UpdateData(FALSE);		
		bClosed=FALSE;
	}
	ShowWindow(SW_NORMAL);
	m_TalkEdit.SetFocus();
	return 0;
}

void CTalkDlg::OnShowDetail() 
{
	if(pState==NULL)return;
	if(pState->pFInfo==NULL)
	{
		pState->pFInfo=new CFriendDetailDlg;
		pState->pFInfo->Create(CFriendDetailDlg::IDD,GetDesktopWindow());
	}
	pState->pFInfo->SendMessage(WM_SHOW_FRIEND_DETAIL,(WPARAM)pInfo,(LPARAM)pState);
}

void CTalkDlg::OnOK() 
{
	if(m_TalkEdit.GetWindowTextLength()==0)
	{
		CString str;
		str.LoadString(IDS_CANNOT_SEND_NULL);
		MessageBox(str);
		return;
	}
	m_TalkEdit.SetReadOnly();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SendMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
	CMsg1 msg;	
	msg.index=SEND_MSG_TO_FRIEND;
	msg.MyId=GetApp()->m_uCurrentUserID;	
	msg.FriendId=pInfo->Id;
	msg.Time=CTime::GetCurrentTime();
	if(pState->OnlineState)
	{
		msg.tarIP=pState->IP;
		msg.nPort=pState->Port;	
	}
	else
	{
		msg.tarIP=GetApp()->m_uServerIP;
		msg.nPort=GetApp()->m_uServerPort;
	}
	m_TalkEdit.GetWindowText(msg.Msg);
	m_bCancel=FALSE;

tryagain:
	if(GetApp()->m_Socket.SendData(&msg,m_bCancel))
	{	
		m_TalkEdit.SetWindowText("");
		m_TalkEdit.SetReadOnly(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		
		CString str;
		str.Format("(%s)%s\r\n%s\r\n\r\n",msg.Time.Format("%Y-%m-%d %H:%M:%S"),GetApp()->m_aUserInfo[0]->Name,msg.Msg);		
		
		m_TalkRecord.SetSel(0,0);
		m_TalkRecord.ReplaceSel(str);
		if(pState->pRecv==NULL)
		{
			pState->pRecv=new CLookDlg;
			pState->pRecv->Create(CLookDlg::IDD,GetDesktopWindow());
		}
		pState->pRecv->m_TalkRecord.SetSel(0,0);
		pState->pRecv->m_TalkRecord.ReplaceSel(str);	
		
		CDialog::OnOK();
		bClosed=TRUE;		
	}
	else if(!m_bCancel)	
	{
		if(msg.tarIP==GetApp()->m_uServerIP&&msg.nPort==GetApp()->m_uServerPort)
		{			
			if(AfxMessageBox(IDS_ASK_RETRY,MB_YESNO)==IDYES)
			{
				goto tryagain;
			}
			else
			{					
				m_TalkEdit.SetReadOnly(FALSE);
				GetDlgItem(IDOK)->EnableWindow();
				m_TalkEdit.SetFocus();
			}
		}
		else
		{
			msg.tarIP=GetApp()->m_uServerIP;
			msg.nPort=GetApp()->m_uServerPort;
			goto tryagain;
		}		
	}
	else
	{
		m_TalkEdit.SetWindowText("");
		m_TalkEdit.SetReadOnly(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CTalkDlg::OnClose() 
{	
	OnCancel();
}

void CTalkDlg::OnCancel() 
{
	bClosed=TRUE;	
	m_bCancel=TRUE;
	CDialog::OnCancel();
}
LRESULT CTalkDlg::OnRefreshData(WPARAM wParam,LPARAM lParam)
{
	m_strEmail=pInfo->Email;
	m_strName=pInfo->Name;
	if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
		m_headBmp.SetBitmap(GetApp()->m_paBitmap[2*pInfo->PhotoId]);
	UpdateData(FALSE);
	return 1;
}

BOOL CTalkDlg::PreTranslateMessage(MSG* pMsg) 
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
