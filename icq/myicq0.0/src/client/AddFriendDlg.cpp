// AddFriendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "AddFriendDlg.h"
#include "Mainfrm.h"
#include "frienddetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFriendDlg dialog


CAddFriendDlg::CAddFriendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFriendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddFriendDlg)
	m_strID = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT
	pDetailDlg=NULL;
	pWnd=NULL;
	bStartAdd=FALSE;
}


void CAddFriendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddFriendDlg)
	DDX_Control(pDX, IDC_FRIEND_DETAIL, m_headbmp);
	DDX_Control(pDX, IDC_RECT, m_Rect);
	DDX_Text(pDX, IDC_ID, m_strID);
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddFriendDlg, CDialog)
	//{{AFX_MSG_MAP(CAddFriendDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FRIEND_DETAIL, OnFriendDetail)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECIEVE_MSG,OnRecvAddFriendRes)
	ON_MESSAGE(WM_NOTICE_ADDFRIEND,OnNoticeAddFriend)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddFriendDlg message handlers

void CAddFriendDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
	{
		EndWaitCursor();
		KillTimer(1);
		CString str;
		str.LoadString(IDS_SERVER_NOT_RES);
		MessageBox(str);
		CDialog::OnOK();
	}
}

LRESULT CAddFriendDlg::OnRecvAddFriendRes(WPARAM wParam,LPARAM lParam)
{
	if(!bStartAdd)return 0;
	bStartAdd=FALSE;
	CData *pData=(CData*)wParam;

	if(pData->index==RE_ADD_AS_FRIEND)
	{
		KillTimer(1);
		CMsg3 msg;
		CopyDataBuf(&msg,pData);
		delete pData;
		if(!msg.LoadFromBuf())return 0;
		//(0,1,2,3 别人拒绝，成功加入，要求验证人份，此人已经为好友)
		EndWaitCursor();
		
		if(msg.Value==0)
		{
			MessageBox("服务器拒绝请求!");
			CDialog::OnOK();
		}
		else if(msg.Value==1)
		{
			CDialog::OnOK();
			CMainFrame * pFrame=(CMainFrame*)GetApp()->m_pMainWnd;
			pFrame->PostMessage(WM_ADDFRIEND_REFRESH,uID,0);			
		}
		else if(msg.Value=2)
		{			
			CRect r1,r2;
			GetWindowRect(&r1);
			m_Rect.GetWindowRect(&r2);
			r1.SetRect(r1.left,r1.top,r1.right,r2.bottom+4);	
			MoveWindow(r1);	
			MessageBox("对方需要身份验证!");
			ShowWindow(SW_NORMAL);
		}
		else
		{
			MessageBox("此人已经在好友列表中!","错误");
			CDialog::OnOK();			
		}
		return 1;
	}
	return 0;
}

LRESULT CAddFriendDlg::OnNoticeAddFriend(WPARAM wParam,LPARAM lParam)
{
	if(uID==0)return 0;
	m_strID.Format("%d",uID);
	m_headbmp.SetBitmap(GetApp()->m_paBitmap[0]);
	
	if(!GetApp()->m_bOnlineState)
	{
		CString str;
		str.LoadString(IDS_NOT_LOGIN_SERVER);
		MessageBox(str);
		CDialog::OnCancel();
		return 0;
	}
	if(FindInArray(GetApp()->m_aUserInfo,uID)!=-1)
	{
		MessageBox("此人已经在好友列表中!","错误");
		CDialog::OnCancel();
		return 0;
	}
	BeginWaitCursor();
	CMsg1 msg;
	msg.index=ADD_AS_FRIEND;
	msg.MyId=GetApp()->m_uCurrentUserID;
	msg.FriendId=uID;
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;
	msg.Time=CTime::GetCurrentTime();
	BOOL bCancel=FALSE;
	if(GetApp()->m_Socket.SendData(&msg,bCancel,this))
	{
		bStartAdd=TRUE;
		SetTimer(1,TimeWaitForAddFriend,NULL);
	}
	else
	{
		CString str;
		str.LoadString(IDS_SERVER_NOT_RES);
		MessageBox(str);
		CDialog::OnCancel();
	}
	return 1;
}

void CAddFriendDlg::OnFriendDetail() 
{
	memset(&Info,0,sizeof(Info));	
	Info.Id=uID;
	Info.HaveDetail=0;
	if(pDetailDlg==NULL)
	{
		pDetailDlg=new CFriendDetailDlg;
		pDetailDlg->Create(CFriendDetailDlg::IDD,GetDesktopWindow());		
	}	
	pDetailDlg->SendMessage(WM_SHOW_FRIEND_DETAIL,(WPARAM)&Info,0);		
	pDetailDlg->OnRefresh();		
}
