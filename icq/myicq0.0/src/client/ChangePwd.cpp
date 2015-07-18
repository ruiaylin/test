// ChangePwd.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "ChangePwd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangePwd dialog


CChangePwd::CChangePwd(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePwd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePwd)
	m_bCanModify = FALSE;
	//}}AFX_DATA_INIT
}


void CChangePwd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePwd)
	DDX_Control(pDX, IDC_OLDPWD, m_oldpwdEdit);
	DDX_Control(pDX, IDC_NEW_PWD2, m_newpwd2Edit);
	DDX_Control(pDX, IDC_NEW_PWD1, m_newpwdEdit);
	DDX_Check(pDX, IDC_START_CHANG, m_bCanModify);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangePwd, CDialog)
	//{{AFX_MSG_MAP(CChangePwd)
	ON_BN_CLICKED(IDC_START_CHANG, OnStartChang)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePwd message handlers

void CChangePwd::OnStartChang() 
{
	UpdateData();
	if(m_bCanModify)
	{
		m_oldpwdEdit.EnableWindow();
		m_newpwdEdit.EnableWindow();
		m_newpwd2Edit.EnableWindow();
	}
	else
	{
		m_oldpwdEdit.EnableWindow(FALSE);
		m_newpwdEdit.EnableWindow(FALSE);
		m_newpwd2Edit.EnableWindow(FALSE);
	}
}

void CChangePwd::OnOK() 
{
	CString strOldpwd,strNewpwd1,strNewpwd2;
	m_oldpwdEdit.GetWindowText(strOldpwd);
	m_newpwdEdit.GetWindowText(strNewpwd1);
	m_newpwd2Edit.GetWindowText(strNewpwd2);
	if(strOldpwd!=GetApp()->m_strCurIDpwd)
	{
		MessageBox("¾ÉÃÜÂë´íÎó!");
		CDialog::OnCancel();
	}
	if(strNewpwd1!=strNewpwd2||strNewpwd1.IsEmpty())
	{
		MessageBox("Á½¸öÐÂÃÜÂë²»Æ¥Åä!");
		m_newpwdEdit.SetWindowText("");
		m_newpwd2Edit.SetWindowText("");
		m_newpwdEdit.SetFocus();
		return;
	}
	CMsgModifyPwd msg;
	msg.index=CHANGE_PASSWORD;
	msg.MyId=GetApp()->m_uCurrentUserID;
	msg.OldPwd=strOldpwd;
	msg.NewPwd=strNewpwd1;
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;
	BOOL bCancel=FALSE;
	if(GetApp()->m_Socket.SendData(&msg,bCancel))
	{
		GetApp()->m_strCurIDpwd=strNewpwd1;
		CDialog::OnOK();
	}
	else
	{
		CString str;
		str.LoadString(IDS_SERVER_NOT_RES);
		MessageBox(str);
	}
}
