// LookDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "LookDlg.h"
#include "mainfrm.h"
#include "talkdlg.h"
#include "frienddetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLookDlg dialog


CLookDlg::CLookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLookDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLookDlg)
	m_strTime = _T("");
	m_strTalk = _T("");
	m_strID = _T("");
	m_strName = _T("");
	m_strEmail = _T("");
	m_strDate = _T("");
	//}}AFX_DATA_INIT
	pInfo=NULL;
	pState=NULL;	
	m_bClose=TRUE;
	hIcon=AfxGetApp()->LoadIcon(IDR_MESSAGE1);
}


void CLookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLookDlg)
	DDX_Control(pDX, ID_SHOW_DETAIL, m_headBmp);
	DDX_Control(pDX, IDC_TALK_RECORD, m_TalkRecord);
	DDX_Text(pDX, IDC_TIME, m_strTime);
	DDX_Text(pDX, IDC_TALK, m_strTalk);
	DDX_Text(pDX, IDC_ID, m_strID);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_EMAIL, m_strEmail);
	DDX_Text(pDX, IDC_DATE, m_strDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLookDlg, CDialog)
	//{{AFX_MSG_MAP(CLookDlg)
	ON_BN_CLICKED(ID_SHOW_TALK, OnShowTalk)
	ON_BN_CLICKED(ID_NEXT, OnNext)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_SHOW_DETAIL, OnShowDetail)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECVMSG,OnRecvMsg)
	ON_MESSAGE(WM_REFRESH_DATA,OnRefreshData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLookDlg message handlers

BOOL CLookDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);	
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);
	CRect r1,r2;
	GetWindowRect(&r1);
	m_TalkRecord.GetWindowRect(&r2);
	r1.SetRect(r1.left,r1.top,r1.right,r2.top);

	MoveWindow(r1);
	return TRUE;  
}

void CLookDlg::OnShowTalk() 
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

LRESULT CLookDlg::OnRecvMsg(WPARAM wParam,LPARAM lParam) 
{	
	pInfo=(UserInfo*)wParam;
	pState=(FriendState*)lParam;
	
	int & iCurMsg=pState->nCurrentMsg;
	
	CString str;
	int i=pState->aMsg.GetSize()-1;
	str.Format("(%s)%s\r\n%s\r\n\r\n",pState->aMsg.GetAt(i)->Time.Format("%Y-%m-%d %H:%M:%S"),pInfo->Name,pState->aMsg.GetAt(i)->Msg);
	m_TalkRecord.SetSel(0,0);
	m_TalkRecord.ReplaceSel(str);
	if(pState->pTalk==NULL)
	{
		pState->pTalk=new CTalkDlg;
		pState->pTalk->Create(CTalkDlg::IDD,GetDesktopWindow());
	}
	pState->pTalk->m_TalkRecord.SetSel(0,0);
	pState->pTalk->m_TalkRecord.ReplaceSel(str);

	if(m_bClose)
	{		
		m_strID.Format("%d",pInfo->Id);
		int photoid=0;
		if(pInfo->HaveDetail)
		{		
			m_strEmail=pInfo->Email;
			m_strName=pInfo->Name;			
			if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
				photoid=pInfo->PhotoId;			
		}
		CBitmap* pBmp=NULL;
		pBmp=GetApp()->m_paBitmap[2*photoid];
		m_headBmp.SetBitmap(pBmp);

		m_strDate=pState->aMsg.GetAt(iCurMsg)->Time.Format(" %Y-%m-%d");
		m_strTime=pState->aMsg.GetAt(iCurMsg)->Time.Format(" %H:%M:%S");
		m_strTalk=pState->aMsg.GetAt(iCurMsg)->Msg;
		
		iCurMsg++;
		if(pState->aMsg.GetSize()>iCurMsg)
			GetDlgItem(ID_NEXT)->EnableWindow();
		else
			GetDlgItem(ID_NEXT)->EnableWindow(FALSE);		
		
		UpdateData(FALSE);
		m_bClose=FALSE;		
	}
	else
	{
		if(pState->aMsg.GetSize()>iCurMsg)
			GetDlgItem(ID_NEXT)->EnableWindow();		
	}
	ShowWindow(SW_SHOWNORMAL);
	return 0;
}

void CLookDlg::OnNext() 
{
	if(pState==NULL)return;
	int& iCurMsg=pState->nCurrentMsg;
	if(pState->aMsg.GetSize()>iCurMsg)
	{		
		m_strDate=pState->aMsg.GetAt(iCurMsg)->Time.Format(" %Y-%m-%d");
		m_strTime=pState->aMsg.GetAt(iCurMsg)->Time.Format(" %H:%M:%S");
		m_strTalk=pState->aMsg.GetAt(iCurMsg)->Msg;		
		
		iCurMsg++;
		if(pState->aMsg.GetSize()>iCurMsg)
			GetDlgItem(ID_NEXT)->EnableWindow();
		else
			GetDlgItem(ID_NEXT)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
}

void CLookDlg::OnOK() 
{
	m_bClose=TRUE;	
	if(pState->pTalk==NULL)
	{
		pState->pTalk=new CTalkDlg;
		pState->pTalk->Create(CTalkDlg::IDD,GetDesktopWindow());
	}
	CRect r1,r2,r3;
	GetWindowRect(&r1);
	pState->pTalk->GetWindowRect(&r2);
	r3.left=r1.left;
	r3.top=r1.top;
	r3.right=r1.left+r2.Width();
	r3.bottom=r1.top+r2.Height();
	pState->pTalk->MoveWindow(&r3,FALSE);
	pState->pTalk->PostMessage(WM_SEND_MSG,(WPARAM)pInfo,(LPARAM)pState);
	
	CDialog::OnOK();
}

void CLookDlg::OnCancel() 
{
	m_bClose=TRUE;	
	CDialog::OnCancel();
}

void CLookDlg::OnClose() 
{
	m_bClose=TRUE;	
	CDialog::OnClose();
}

void CLookDlg::OnShowDetail() 
{
	if(pState->pFInfo==NULL)
	{
		pState->pFInfo=new CFriendDetailDlg;
		pState->pFInfo->Create(CFriendDetailDlg::IDD,GetDesktopWindow());
	}
	pState->pFInfo->SetOwnerWnd(this);
	pState->pFInfo->SendMessage(WM_SHOW_FRIEND_DETAIL,(WPARAM)pInfo,(LPARAM)pState);
	pState->pFInfo->OnRefresh();
}

LRESULT CLookDlg::OnRefreshData(WPARAM wParam,LPARAM lParam)
{
	m_strEmail=pInfo->Email;
	m_strName=pInfo->Name;
	if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
		m_headBmp.SetBitmap(GetApp()->m_paBitmap[2*pInfo->PhotoId]);
	UpdateData(FALSE);
	return 1;
}