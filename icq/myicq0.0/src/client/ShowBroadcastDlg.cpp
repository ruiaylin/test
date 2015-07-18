// ShowBroadcastDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "ShowBroadcastDlg.h"
#include "FriendDetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowBroadcastDlg dialog


CShowBroadcastDlg::CShowBroadcastDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowBroadcastDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowBroadcastDlg)
	m_strDate = _T("");
	m_strEmail = _T("");
	m_strID = _T("");
	m_strName = _T("");
	m_strTime = _T("");
	//}}AFX_DATA_INIT
	hIcon=AfxGetApp()->LoadIcon(IDR_MESSAGE);	
	pDetailDlg=NULL;
	pInfo=NULL;
	m_bClose=TRUE;
}


void CShowBroadcastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowBroadcastDlg)
	DDX_Control(pDX, ID_SHOW_DETAIL, m_headbmp);
	DDX_Control(pDX, IDC_TALK, m_EditMsg);
	DDX_Control(pDX, IDC_HISTORY, m_EditRecord);
	DDX_Text(pDX, IDC_DATE, m_strDate);
	DDX_Text(pDX, IDC_EMAIL, m_strEmail);
	DDX_Text(pDX, IDC_ID, m_strID);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_TIME, m_strTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShowBroadcastDlg, CDialog)
	//{{AFX_MSG_MAP(CShowBroadcastDlg)
	ON_BN_CLICKED(ID_NEXT, OnNext)
	ON_BN_CLICKED(ID_SHOW_HISTORY, OnShowHistory)
	ON_BN_CLICKED(ID_SHOW_DETAIL, OnShowDetail)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOW_NOTICE,OnShowNotice)	
	ON_MESSAGE(WM_REFRESH_DATA,OnRefreshData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowBroadcastDlg message handlers

BOOL CShowBroadcastDlg::OnInitDialog() 
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
	m_EditRecord.GetWindowRect(&r2);	
	r1.SetRect(r1.left,r1.top,r1.right,r2.top);

	MoveWindow(r1);	
	return TRUE; 
}

void CShowBroadcastDlg::OnNext() 
{		
	SaveMsg * pMsg=GetApp()->m_aNoticeMsg.GetAt(GetApp()->m_nCurNotice);
	if(GetApp()->m_aNoticeMsg.GetSize()>GetApp()->m_nCurNotice)
	{		
		memset(&Info,0,sizeof(Info));
		Info.Id=pMsg->ID;
		Info.HaveDetail=0;
		UserInfo * pInfo=NULL;
		m_strID.Format("%d",pMsg->ID);
		int photoid=0;			
		if((FindInArray(GetApp()->m_aUserInfo,pMsg->ID,pInfo)||FindInArray(GetApp()->m_aStranger,pMsg->ID,pInfo))&&pInfo->HaveDetail)
		{			
			m_strEmail=pInfo->Email;
			m_strName=pInfo->Name;
			if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
				photoid=pInfo->PhotoId;			
		}
		else
		{
			m_strEmail="";
			m_strName="";
			pInfo=&Info;
		}
		CBitmap* pBmp=NULL;
		pBmp=GetApp()->m_paBitmap[2*photoid];
		m_headbmp.SetBitmap(pBmp);

		m_strDate=pMsg->Time.Format(" %Y-%m-%d");
		m_strTime=pMsg->Time.Format(" %H:%M:%S");
		m_EditMsg.SetWindowText(pMsg->Msg);		
		
		GetApp()->m_nCurNotice++;
		if(GetApp()->m_aNoticeMsg.GetSize()>GetApp()->m_nCurNotice)
			GetDlgItem(ID_NEXT)->EnableWindow();
		else
			GetDlgItem(ID_NEXT)->EnableWindow(FALSE);
		UpdateData(FALSE);
	} 	
}

void CShowBroadcastDlg::OnShowHistory() 
{
	static BOOL bShowRecord=TRUE;
	
	CRect r1,r2;
	GetWindowRect(&r1);
	m_EditRecord.GetWindowRect(&r2);

	if(bShowRecord)
		r1.SetRect(r1.left,r1.top,r1.right,r2.bottom+4);	
	else
		r1.SetRect(r1.left,r1.top,r1.right,r2.top);
	bShowRecord=!bShowRecord;
	MoveWindow(r1);	 	
}

void CShowBroadcastDlg::OnShowDetail() 
{
	if(pDetailDlg==NULL)
	{
		pDetailDlg=new CFriendDetailDlg;
		pDetailDlg->Create(CFriendDetailDlg::IDD,GetDesktopWindow());		
	}
	pDetailDlg->SetOwnerWnd(this);
	pDetailDlg->SendMessage(WM_SHOW_FRIEND_DETAIL,(WPARAM)pInfo,0);
	pDetailDlg->OnRefresh();
}

LRESULT CShowBroadcastDlg::OnShowNotice(WPARAM wParam,LPARAM lParam)
{	
	CString str;	
	SaveMsg *pMsgNew=GetApp()->m_aNoticeMsg.GetAt(GetApp()->m_aNoticeMsg.GetSize()-1);	
	str.Format("(%s)%d\r\n%s\r\n\r\n",pMsgNew->Time.Format("%Y-%m-%d %H:%M:%S"),pMsgNew->ID,pMsgNew->Msg);
	m_EditRecord.SetSel(0,0);
	m_EditRecord.ReplaceSel(str);	
	
	SaveMsg* pMsg=GetApp()->m_aNoticeMsg.GetAt(GetApp()->m_nCurNotice);
	
	if(m_bClose)
	{		
		memset(&Info,0,sizeof(Info));
		Info.Id=pMsg->ID;
		Info.HaveDetail=0;		
		m_strID.Format("%d",pMsg->ID);
		int photoid=0;		
		if((FindInArray(GetApp()->m_aUserInfo,pMsg->ID,pInfo)||FindInArray(GetApp()->m_aStranger,pMsg->ID,pInfo))&&pInfo->HaveDetail)
		{			
			m_strEmail=pInfo->Email;
			m_strName=pInfo->Name;
			if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
				photoid=pInfo->PhotoId;			
		}
		else
		{
			m_strEmail="";
			m_strName="";
			pInfo=&Info;
		}		
		m_headbmp.SetBitmap(GetApp()->m_paBitmap[2*photoid]);

		m_strDate=pMsg->Time.Format(" %Y-%m-%d");
		m_strTime=pMsg->Time.Format(" %H:%M:%S");
		m_EditMsg.SetWindowText(pMsg->Msg);
		
		GetApp()->m_nCurNotice++;
		if(GetApp()->m_aNoticeMsg.GetSize()>GetApp()->m_nCurNotice)
			GetDlgItem(ID_NEXT)->EnableWindow();
		else
			GetDlgItem(ID_NEXT)->EnableWindow(FALSE);		
		
		UpdateData(FALSE);
		m_bClose=FALSE;		
	}
	else
	{
		if(GetApp()->m_aNoticeMsg.GetSize()>GetApp()->m_nCurNotice)
			GetDlgItem(ID_NEXT)->EnableWindow();		
	}
	
	ShowWindow(SW_SHOWNORMAL);
	return 0;
}

void CShowBroadcastDlg::OnCancel() 
{
	m_bClose=TRUE;	
	CDialog::OnCancel();
}
void CShowBroadcastDlg::OnOK()
{
	OnCancel();
}
void CShowBroadcastDlg::OnClose() 
{		
	OnCancel();	
}
LRESULT CShowBroadcastDlg::OnRefreshData(WPARAM wParam,LPARAM lParam)
{
	m_strName=pInfo->Name;
	m_strEmail=pInfo->Email;
	if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
		m_headbmp.SetBitmap(GetApp()->m_paBitmap[2*pInfo->PhotoId]);
	UpdateData(FALSE);
	return 1;
}