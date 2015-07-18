// FriendDetail.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "FriendDetail.h"
#include "Mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFriendDetailDlg dialog


CFriendDetailDlg::CFriendDetailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFriendDetailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFriendDetailDlg)
	m_strAddress = _T("");
	m_strAge = _T("");
	m_strDepartment = _T("");
	m_strDescription = _T("");
	m_strEmail = _T("");
	m_strFax = _T("");
	m_strHomepage = _T("");
	m_strID = _T("");
	m_strName = _T("");
	m_strPhone = _T("");
	m_canbeadd = -1;
	//}}AFX_DATA_INIT
	pInfo=NULL;	
	pWnd=NULL;
	hIcon=AfxGetApp()->LoadIcon(IDR_NOTIFY);
}


void CFriendDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFriendDetailDlg)
	DDX_Control(pDX, IDC_SEX, m_ComboSex);
	DDX_Control(pDX, IDC_HEADBMP, m_ComboFace);
	DDX_Text(pDX, IDC_ADDRESS, m_strAddress);
	DDX_Text(pDX, IDC_AGE, m_strAge);
	DDX_Text(pDX, IDC_DEPARTMENT, m_strDepartment);
	DDX_Text(pDX, IDC_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_EMAIL, m_strEmail);
	DDX_Text(pDX, IDC_FAX, m_strFax);
	DDX_Text(pDX, IDC_HOMEPAGE, m_strHomepage);
	DDX_Text(pDX, IDC_ID, m_strID);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_PHONE, m_strPhone);
	DDX_Radio(pDX, IDC_ALLOW_ALL, m_canbeadd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFriendDetailDlg, CDialog)
	//{{AFX_MSG_MAP(CFriendDetailDlg)
	ON_BN_CLICKED(ID_REFRESH, OnRefresh)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOW_FRIEND_DETAIL,OnShowFriendDetail)
	ON_MESSAGE(WM_RECIEVE_MSG,OnRecvFriendDetail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFriendDetailDlg message handlers

void CFriendDetailDlg::OnRefresh() 
{
	if(!GetApp()->m_bOnlineState)
		return;

	CMsg1 msg;
	msg.index=FRIEND_DETAIL;
	msg.MyId=GetApp()->m_uCurrentUserID;
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;
	msg.FriendId=pInfo->Id;	
	BOOL bCancel=FALSE;
	if(!GetApp()->m_Socket.SendData(&msg,bCancel,this))
	{
		CString str;
		str.LoadString(IDS_SERVER_NOT_RES);
		MessageBox(str);
		OnOK();		
		return;
	}
	BeginWaitCursor();
	SetTimer(2,TimeWaitForFriendDetail,NULL);
}
LRESULT CFriendDetailDlg::OnShowFriendDetail(WPARAM wParam,LPARAM lParam)
{
	pInfo=(UserInfo*)wParam;	
	ASSERT(pInfo!=NULL);	
	
	m_strID.Format("%d",pInfo->Id);	
	int photoid=0;
	if(pInfo->HaveDetail)
	{
		if(pInfo->CanbeAdd==0)m_canbeadd=2;
		else if(pInfo->CanbeAdd==1)m_canbeadd=0;
		else m_canbeadd=1;
		m_strAddress=pInfo->Address;
		m_strAge=pInfo->Age;
		m_strDepartment=pInfo->Department;
		m_strDescription=pInfo->Description;
		m_strEmail=pInfo->Email;
		m_strFax=pInfo->Fax;
		m_strHomepage=pInfo->Homepage;		
		m_strName=pInfo->Name;
		m_strPhone=pInfo->Phone;		
		if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
			photoid=pInfo->PhotoId;		
		if(pInfo->Sex==0)m_ComboSex.SetCurSel(0);
		else if(pInfo->Sex==1)m_ComboSex.SetCurSel(1);		
	}
	m_ComboFace.SetCurSel(photoid);
	UpdateData(FALSE);
	ShowWindow(SW_NORMAL);

	return 0;
}

BOOL CFriendDetailDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);	
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);
 	
	m_ComboFace.SetImageList(&GetApp()->m_imaHead);
	return TRUE;
}

void CFriendDetailDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==2)
	{
		EndWaitCursor();
		KillTimer(2);
		CString str;
		str.LoadString(IDS_SERVER_NOT_RES);
		MessageBox(str);
		OnOK();
	}	
}

LRESULT CFriendDetailDlg::OnRecvFriendDetail(WPARAM wParam,LPARAM lParam)
{
	if(!GetApp()->m_bOnlineState)return 0;

	CData* pData=(CData*)wParam;
	if(pData->index==RE_FRIEND_DETAIL)
	{
		if(pInfo==NULL)return 0;
		CMsgPerson msg;
		CopyDataBuf(&msg,pData);
		if(!msg.LoadFromBuf())return 0;
		
		pInfo->HaveDetail=1;
		pInfo->CanbeAdd=msg.Canbeadd;
		pInfo->Sex=msg.Sex;
		pInfo->PhotoId=msg.PhotoId;
		lstrcpy(pInfo->Address,msg.Address);
		lstrcpy(pInfo->Age,msg.Age);			
		lstrcpy(pInfo->Department,msg.Department);
		lstrcpy(pInfo->Description,msg.Description);
		lstrcpy(pInfo->Email,msg.Email);
		lstrcpy(pInfo->Fax,msg.Fax);
		lstrcpy(pInfo->Homepage,msg.Homepage);
		lstrcpy(pInfo->Phone,msg.Phone);
		lstrcpy(pInfo->Name,msg.Name);			

		UserInfo *pInfo2=NULL;
		if(FindInArray(GetApp()->m_aUserInfo,msg.MyId,pInfo2)||FindInArray(GetApp()->m_aStranger,msg.MyId,pInfo2))
		{
			if(pInfo!=pInfo2)
			{
				pInfo2->HaveDetail=1;
				pInfo2->CanbeAdd=msg.Canbeadd;
				pInfo2->Sex=msg.Sex;
				pInfo2->PhotoId=msg.PhotoId;
				lstrcpy(pInfo2->Address,msg.Address);
				lstrcpy(pInfo2->Age,msg.Age);			
				lstrcpy(pInfo2->Department,msg.Department);
				lstrcpy(pInfo2->Description,msg.Description);
				lstrcpy(pInfo2->Email,msg.Email);
				lstrcpy(pInfo2->Fax,msg.Fax);
				lstrcpy(pInfo2->Homepage,msg.Homepage);
				lstrcpy(pInfo2->Phone,msg.Phone);
				lstrcpy(pInfo2->Name,msg.Name);		
			}
			CMainFrame *pFrame=(CMainFrame*)GetApp()->m_pMainWnd;
			pFrame->m_wndBar.ModifyItem(pInfo->Id,pInfo->PhotoId,pInfo->Name);
			pFrame->m_wndBar.Invalidate();
		}		
		
		m_strID.Format("%d",pInfo->Id);			
		if(pInfo->CanbeAdd==0)m_canbeadd=2;
		else if(pInfo->CanbeAdd==1)m_canbeadd=0;
		else m_canbeadd=1;
		m_strAddress=pInfo->Address;
		m_strAge=pInfo->Age;
		m_strDepartment=pInfo->Department;
		m_strDescription=pInfo->Description;
		m_strEmail=pInfo->Email;
		m_strFax=pInfo->Fax;
		m_strHomepage=pInfo->Homepage;		
		m_strName=pInfo->Name;
		m_strPhone=pInfo->Phone;					
		if(pInfo->Sex==0)m_ComboSex.SetCurSel(0);
		else if(pInfo->Sex==1)m_ComboSex.SetCurSel(1);		
		int photoid=0;
		if(pInfo->PhotoId>=0&&pInfo->PhotoId<UINT(GetApp()->m_nNumberHeadBmp))
			photoid=pInfo->PhotoId;
		m_ComboFace.SetCurSel(photoid);
		UpdateData(FALSE);
		EndWaitCursor();
		KillTimer(2);
		if(pWnd!=NULL)pWnd->PostMessage(WM_REFRESH_DATA);
		return 1;
	}
	else
	{
		return 0;
	}
}

void CFriendDetailDlg::OnCancel() 
{
	if(pWnd!=NULL)pWnd->PostMessage(WM_REFRESH_DATA);
	CDialog::OnCancel();
}

void CFriendDetailDlg::OnClose() 
{ 	
	OnCancel();	
}
