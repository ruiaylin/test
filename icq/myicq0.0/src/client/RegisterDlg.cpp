// RegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "RegisterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog


CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterDlg)
	m_strServerIP = _T("");
	m_uServerPort = 0;
	//}}AFX_DATA_INIT
	m_bApplyID=TRUE;
	hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterDlg)
	DDX_Control(pDX, IDC_HEADBMP, m_headbmp);
	DDX_Control(pDX, IDC_USER_PWD, m_userpwd);
	DDX_Control(pDX, IDC_USER_ID, m_userid);
	DDX_Control(pDX, IDC_SEX, m_sex);
	DDX_Control(pDX, IDC_PWD1, m_pwd1);
	DDX_Control(pDX, IDC_PWD, m_pwd);
	DDX_Control(pDX, IDC_PHONE, m_phone);
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_HOMEPAGE, m_homepage);
	DDX_Control(pDX, IDC_FAX, m_fax);
	DDX_Control(pDX, IDC_EMAIL, m_email);
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_DEPARTMENT, m_department);
	DDX_Control(pDX, IDC_APPLY_ID, m_applyid);
	DDX_Control(pDX, IDC_ALLOW_ALL, m_allowall);
	DDX_Control(pDX, IDC_NEED_CHECK, m_needcheck);
	DDX_Control(pDX, IDC_NOT_ALLOW, m_notallow);
	DDX_Control(pDX, IDC_AGE, m_age);
	DDX_Control(pDX, IDC_ADDRESS, m_address);
	DDX_Text(pDX, IDC_SERVERIP, m_strServerIP);
	DDX_Text(pDX, IDC_SERVER_PORT, m_uServerPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	//{{AFX_MSG_MAP(CRegisterDlg)
	ON_BN_CLICKED(IDC_USE_HAVEID, OnUseHaveid)
	ON_BN_CLICKED(IDC_APPLY_ID, OnApplyId)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECIEVE_MSG,OnRecvMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg message handlers

BOOL CRegisterDlg::OnInitDialog() 
{	
	CDialog::OnInitDialog();

	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);
		
	m_applyid.SetCheck(1);
	m_allowall.SetCheck(1);	

	m_description.SetWindowText("这家伙很懒，什么都没留下");
	CString temp;
	
	m_headbmp.SetImageList(&GetApp()->m_imaHead);
	m_headbmp.SetCurSel(0);	

	UpdateData(FALSE);
	return TRUE; 
}

void CRegisterDlg::OnUseHaveid() 
{	
	m_userid.EnableWindow();
	m_userpwd.EnableWindow();	
	
	m_bApplyID=FALSE;
	m_sex.EnableWindow(FALSE);	
	m_pwd1.EnableWindow(FALSE);
	m_pwd.EnableWindow(FALSE);
	m_phone.EnableWindow(FALSE);
	m_name.EnableWindow(FALSE);
	m_homepage.EnableWindow(FALSE);
	m_headbmp.EnableWindow(FALSE);
	m_fax.EnableWindow(FALSE);
	m_email.EnableWindow(FALSE);
	m_description.EnableWindow(FALSE);
	m_department.EnableWindow(FALSE);
	CButton* pB=(CButton*)GetDlgItem(IDC_NEED_CHECK);
	pB->EnableWindow(FALSE);
	pB=(CButton*)GetDlgItem(IDC_NOT_ALLOW);
	pB->EnableWindow(FALSE);
	m_needcheck.EnableWindow(FALSE);
	m_notallow.EnableWindow(FALSE);
	m_allowall.EnableWindow(FALSE);
	m_age.EnableWindow(FALSE);
	m_address.EnableWindow(FALSE);
}

void CRegisterDlg::OnApplyId() 
{
	m_userid.EnableWindow(FALSE);
	m_userpwd.EnableWindow(FALSE);	
	
	m_bApplyID=TRUE;
	CButton* pB=(CButton*)GetDlgItem(IDC_NEED_CHECK);
	pB->EnableWindow();
	pB=(CButton*)GetDlgItem(IDC_NOT_ALLOW);
	pB->EnableWindow();
	m_sex.EnableWindow();	
	m_pwd1.EnableWindow();
	m_pwd.EnableWindow();
	m_phone.EnableWindow();
	m_name.EnableWindow();
	m_homepage.EnableWindow();
	m_headbmp.EnableWindow();
	m_fax.EnableWindow();
	m_email.EnableWindow();
	m_description.EnableWindow();
	m_department.EnableWindow();	
	m_allowall.EnableWindow();
	m_age.EnableWindow();
	m_address.EnableWindow();	
}

LRESULT CRegisterDlg::OnRecvMsg(WPARAM wParam,LPARAM lParam)
{
	CData* pData=(CData*)wParam;
	if(pData->index==APPLY_ID_OK)
	{
		KillTimer(1);
		EndWaitCursor();		
		m_uUserID=pData->MyId;
		CString str;
		str.Format("恭喜您注册成功！您的ID号为：%d!",pData->MyId);
		MessageBox(str,"注册成功");
		CDialog::OnOK();
		UserInfo* pInfo=new UserInfo;
		pInfo->Id=pData->MyId;
		lstrcpy(pInfo->Name,msg.Name);
		lstrcpy(pInfo->Address,msg.Address);
		lstrcpy(pInfo->Age,msg.Age);
		pInfo->CanbeAdd=msg.Canbeadd;
		lstrcpy(pInfo->Department,msg.Department);
		lstrcpy(pInfo->Description,msg.Description);
		lstrcpy(pInfo->Email,msg.Email);
		lstrcpy(pInfo->Fax,msg.Fax);
		lstrcpy(pInfo->Homepage,msg.Homepage);
		pInfo->HaveDetail=1;
		lstrcpy(pInfo->Phone,msg.Phone);
		pInfo->PhotoId=msg.PhotoId;
		pInfo->Sex=msg.Sex;
		GetApp()->m_aUserInfo.Add(pInfo);
		FriendState* pState=new FriendState;
		GetApp()->m_aUserState.Add(pState);
	}
	else if(pData->index==RE_LOGIN_INFO)
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
				UserInfo *pInfo=new UserInfo;
				pInfo->Id=msg.MyId;
				CString str;
				str.Format("%d",m_userid);
				lstrcpy(pInfo->Name,str);
				pInfo->HaveDetail=0;
				GetApp()->m_aUserInfo.Add(pInfo);
				FriendState* pState=new FriendState;
				GetApp()->m_aUserState.Add(pState);
			}
			else if(msg.Value==1)
			{	
				CString str;
				str.LoadString(IDS_PWD_ERROR);
				MessageBox(str);								
			}
			else
			{			
				CString str;
				str.LoadString(IDS_ID_NOT_EXIST);
				MessageBox(str);
			}
		}
	}
	delete pData;
	return 0;	
}

void CRegisterDlg::OnOK() 
{
	UpdateData();
	if(m_strServerIP=="")
	{
		CString str;
		str.LoadString(IDS_SERVER_IP_NOT_NULL);
		MessageBox(str);
		return;
	}
	
	m_uServerIP=inet_addr(m_strServerIP);	

	CClientApp* pApp=(CClientApp*)AfxGetApp();
	BeginWaitCursor();
	if(m_applyid.GetCheck())
	{
		
		CString pwd,pwd1;
		
		m_pwd.GetWindowText(pwd);
		m_pwd1.GetWindowText(pwd1);
		if(pwd.IsEmpty()||pwd!=pwd1)
		{
			EndWaitCursor();
			CString str;
			str.LoadString(IDS_FILL_PWD_ERROR);
			MessageBox(str);
			m_pwd.SetWindowText("");
			m_pwd1.SetWindowText("");
			m_pwd.SetFocus();			
			return;
		}
		m_strPassword=pwd;
		CString str;
		msg.tarIP=m_uServerIP;
		msg.nPort=m_uServerPort;
		msg.index=APPLY_ID_LOGIN;
		m_address.GetWindowText(msg.Address);
		m_age.GetWindowText(msg.Age);
		m_department.GetWindowText(msg.Department);
		m_description.GetWindowText(msg.Description);
		m_email.GetWindowText(msg.Email);
		m_fax.GetWindowText(msg.Fax);
		m_homepage.GetWindowText(msg.Homepage);
		m_name.GetWindowText(msg.Name);
		m_phone.GetWindowText(msg.Phone);
		msg.Password=pwd;
		if(m_allowall.GetCheck())msg.Canbeadd=1;
		else if(m_needcheck.GetCheck())msg.Canbeadd=2;
		else msg.Canbeadd=0;
		msg.PhotoId=m_headbmp.GetCurSel();
		int sex_sel;
		sex_sel=m_sex.GetCurSel();
		if(sex_sel==CB_ERR)sex_sel=2;
		msg.Sex=sex_sel;
		SetTimer(1,TimeWaitForRes+1000,NULL);
		pApp->m_Socket.SendDataDirect(&msg);
	}
	else
	{
		CString str;
		m_userid.GetWindowText(str);
		m_uUserID=atol(LPCSTR(str));
		if(!m_uUserID)
		{
			EndWaitCursor();
			CString str;
			str.LoadString(IDS_FILL_ID_ERROR);
			MessageBox(str);
			m_userid.SetWindowText("");
			m_userid.SetFocus();
			return;
		}

		m_userpwd.GetWindowText(m_strPassword);

		CMsgModifyPwd msg;
		msg.index=HAVE_ID_LOGIN;
		msg.MyId=m_uUserID;
		msg.tarIP=m_uServerIP;
		msg.nPort=m_uServerPort;
		msg.OldPwd=m_strPassword;
		SetTimer(1,TimeWaitForRes+1000,NULL);
		pApp->m_Socket.SendDataDirect(&msg);
	}
}

void CRegisterDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
	{		
		EndWaitCursor();
		KillTimer(1);
		CString str;
		str.LoadString(IDS_SERVER_NOT_RES);
		MessageBox(str);		
	}	
}
