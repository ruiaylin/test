// ModifyPIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "ModifyPIDlg.h"
#include "changepwd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyPIDlg dialog


CModifyPIDlg::CModifyPIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyPIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyPIDlg)
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
	hIcon=AfxGetApp()->LoadIcon(IDR_NOTIFY);
}


void CModifyPIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyPIDlg)
	DDX_Control(pDX, IDC_SEX, m_ComboSex);
	DDX_Control(pDX, IDC_HEADBMP, m_ComboFace);
	DDX_Text(pDX, IDC_ADDRESS, m_strAddress);
	DDX_Text(pDX, IDC_AGE, m_strAge);
	DDV_MaxChars(pDX, m_strAge, 6);
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


BEGIN_MESSAGE_MAP(CModifyPIDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyPIDlg)
	ON_BN_CLICKED(ID_REFRESH, OnRefresh)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_SHOW_CHANGEPWD, OnShowChangepwd)
	ON_BN_CLICKED(ID_MODIFYPI, OnModifypi)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOW_FRIEND_DETAIL,OnShowFriendDetail)
	ON_MESSAGE(WM_RECIEVE_MSG,OnRecvMyDetail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyPIDlg message handlers

void CModifyPIDlg::OnRefresh() 
{
	if(!GetApp()->m_bOnlineState)return;

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
LRESULT CModifyPIDlg::OnShowFriendDetail(WPARAM wParam,LPARAM lParam)
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
		photoid=pInfo->PhotoId;
		if(pInfo->Sex==0)m_ComboSex.SetCurSel(0);
		else if(pInfo->Sex==1)m_ComboSex.SetCurSel(1);		
	}
	m_ComboFace.SetCurSel(photoid);
	UpdateData(FALSE);
	ShowWindow(SW_NORMAL);

	return 0;
}


BOOL CModifyPIDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);	
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);

	
	m_ComboFace.SetImageList(&GetApp()->m_imaHead);	
	m_ComboFace.SetCurSel(0);
 	
	return TRUE;
}

void CModifyPIDlg::OnTimer(UINT nIDEvent) 
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

void CModifyPIDlg::OnShowChangepwd() 
{
	CChangePwd dlg(this);
	dlg.DoModal();
}

LRESULT CModifyPIDlg::OnRecvMyDetail(WPARAM wParam,LPARAM lParam)
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
		return 1;
	}
	else
	{
		return 0;
	}
}

void CModifyPIDlg::OnModifypi() 
{
	if(!GetApp()->m_bOnlineState)return;
	if(pInfo==NULL)return;

	UpdateData();
	CMsgChangePI msg;
	msg.index=CHANGE_PERSONAL_INFO;
	msg.MyId=GetApp()->m_uCurrentUserID;
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;
	if(pInfo->Address!=m_strAddress)
	{
		msg.Address=m_strAddress;
		msg.Mask|=CMsgChangePI::ADDRESS;
	}
	if(pInfo->Age!=m_strAge)
	{
		msg.Age=m_strAge;
		msg.Mask|=CMsgChangePI::AGE;
	}
	int CanbeAdd=1;
	if(m_canbeadd==1)CanbeAdd=2;
	else if(m_canbeadd==2)CanbeAdd=0;
	if(pInfo->CanbeAdd!=CanbeAdd)
	{
		msg.Canbeadd=CanbeAdd;
		msg.Mask|=CMsgChangePI::CANBEADD;
	}
	if(pInfo->Department!=m_strDepartment)
	{
		msg.Department=m_strDepartment;
		msg.Mask|=CMsgChangePI::DEPARTMENT;
	}
	if(pInfo->Description!=m_strDescription)
	{
		msg.Description=m_strDescription;
		msg.Mask|=CMsgChangePI::DESCRIPTION;
	}
	if(pInfo->Email!=m_strEmail)
	{
		msg.Email=m_strEmail;
		msg.Mask|=CMsgChangePI::EMAIL;
	}
	if(pInfo->Fax!=m_strFax)
	{
		msg.Fax=m_strFax;
		msg.Mask|=CMsgChangePI::FAX;
	}
	if(pInfo->Homepage!=m_strHomepage)
	{
		msg.Homepage=m_strHomepage;
		msg.Mask|=CMsgChangePI::HOMEPAGE;
	}
	if(pInfo->Name!=m_strName)
	{
		msg.Name=m_strName;
		msg.Mask|=CMsgChangePI::NAME;
	}
	if(pInfo->Phone!=m_strPhone)
	{
		msg.Phone=m_strPhone;
		msg.Mask|=CMsgChangePI::PHONE;
	}
	BYTE Sex=m_ComboSex.GetCurSel();
	if(Sex!=0&&Sex!=1)Sex=2;
	if(pInfo->Sex!=Sex)
	{
		msg.Sex=Sex;
		msg.Mask|=CMsgChangePI::SEX;
	}
	int photoid=m_ComboFace.GetCurSel();
	if(photoid>=0&&pInfo->PhotoId!=UINT(photoid))
	{
		msg.PhotoId=photoid;
		msg.Mask|=CMsgChangePI::PHOTOID;
	}
	if(msg.Mask==0)return;
		
	BeginWaitCursor();
	BOOL bCancel=FALSE;
	if(GetApp()->m_Socket.SendData(&msg,bCancel))
	{
		if(msg.Mask&CMsgChangePI::ADDRESS)lstrcpy(pInfo->Address,m_strAddress);
		if(msg.Mask&CMsgChangePI::AGE)lstrcpy(pInfo->Age,m_strAge);
		if(msg.Mask&CMsgChangePI::DEPARTMENT)lstrcpy(pInfo->Department,m_strDepartment);
		if(msg.Mask&CMsgChangePI::DESCRIPTION)lstrcpy(pInfo->Description,m_strDescription);
		if(msg.Mask&CMsgChangePI::EMAIL)lstrcpy(pInfo->Email,m_strEmail);
		if(msg.Mask&CMsgChangePI::FAX)lstrcpy(pInfo->Fax,m_strFax);
		if(msg.Mask&&CMsgChangePI::HOMEPAGE)lstrcpy(pInfo->Homepage,m_strHomepage);
		if(msg.Mask&CMsgChangePI::NAME)lstrcpy(pInfo->Name,m_strName);
		if(msg.Mask&CMsgChangePI::PHONE)lstrcpy(pInfo->Phone,m_strPhone);
		if(msg.Mask&CMsgChangePI::SEX)pInfo->Sex=msg.Sex;		
		if(msg.Mask&CMsgChangePI::PHOTOID)pInfo->PhotoId=msg.PhotoId;
		if(msg.Mask&CMsgChangePI::CANBEADD)pInfo->CanbeAdd=msg.Canbeadd;		
		EndWaitCursor();
		MessageBox("修改数据成功！");
		OnOK();		
	}
	else
	{
		EndWaitCursor();
		MessageBox("服务器没有响应!");
	}
}
