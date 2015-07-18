// Server.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Server.h"

#include "MainFrm.h"
#include "ServerDoc.h"
#include "ServerView.h"
#include "msg.h"
#include "setupdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerApp

BEGIN_MESSAGE_MAP(CServerApp, CWinApp)
	//{{AFX_MSG_MAP(CServerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_SETUP_SERVER, OnSetupServer)
	ON_COMMAND(ID_START_SERVER, OnStartServer)
	ON_UPDATE_COMMAND_UI(ID_START_SERVER, OnUpdateStartServer)
	ON_COMMAND(ID_USER_ONLINE, OnUserOnline)
	ON_COMMAND(ID_CLOSE_SERVER, OnCloseServer)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_SERVER, OnUpdateCloseServer)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_UPDATE_COMMAND_UI(ID_USER_ONLINE, OnUpdateUserOnline)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerApp construction

CServerApp::CServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bServerStarted=FALSE;	
	m_nRecvMsg=0;
	m_nNumberOnline=0;
	m_nMaxUserId=0;
	m_pUsers=NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CServerApp object

CServerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CServerApp initialization

BOOL CServerApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	LoadMyProfileSettings();
	
	if (!AfxSocketInit())
	{
		AfxMessageBox("初始化Socket出错");
		return FALSE;
	}

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CServerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CServerView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	char name[20];
	hostent * hostinfo=NULL;
	if(gethostname(name,sizeof(name)) == 0)
	{
		hostinfo = gethostbyname(name);
		if(hostinfo!= NULL)
		{
			m_strLocalIP= inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
		}
	}
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CServerApp message handlers


void CServerApp::OnSetupServer() 
{
	CSetupDlg dlg;
	//dlg.m_strIP=
	dlg.m_strUid=m_strDataSUID;
	dlg.m_strPwd=m_strDataSPwd;
	dlg.m_strDS=m_strDataSource;
	dlg.m_uSendNum=m_nSendNum;
	dlg.m_strBroadcastPwd=m_strBroadcastPwd;
	CString str;
	for(int i=0;i<m_anPort.GetSize();i++)
	{
		str.Format("%d",m_anPort.GetAt(i));
		dlg.m_aStr.Add(str);
	}
	dlg.DoModal();	
}

void CServerApp::OnStartServer() 
{
	if(m_bServerStarted)return;	

	m_pUsers=NULL;
	m_pUsers=new UserOnline[MaxUserNumber];
	for(int i=0;i<MaxUserNumber;i++)
	{
		m_pUsers[i].CanbeAdd=0;
		m_pUsers[i].Id=0;
		m_pUsers[i].IP=0;
		m_pUsers[i].PhotoId=0;
		m_pUsers[i].Port=0;
		m_pUsers[i].State=0;
	}

	try
	{
		m_Database.OpenEx(m_sConnectString);
	}
	catch(CDBException e)
	{
		AfxMessageBox(e.m_strError);
		return;
	}
	
	m_nNumberOnline=0;
	m_nMaxUserId=0;
	m_nRecvMsg=0;

	CRecordset recordset(&m_Database);	

	try
	{
		recordset.Open(AFX_DB_USE_DEFAULT_TYPE,"select max(id) from users");
	}
	catch(CDBException e)
	{
		AfxMessageBox(e.m_strError);
		return;
	}	
	
	if(!recordset.IsEOF())
	{
		CDBVariant value; 
		recordset.GetFieldValue(short(0),value);		
		m_nMaxUserId=value.m_lVal;
		if(m_nMaxUserId<UserIdRadix)m_nMaxUserId=UserIdRadix-1;
		m_nTotalUserNumber=m_nMaxUserId-UserIdRadix+1;
	}
	else
	{
		m_nMaxUserId=UserIdRadix-1;
		m_nTotalUserNumber=0;
	}
	recordset.Close();

	try
	{
		recordset.Open(AFX_DB_USE_DEFAULT_TYPE,"select id,photoid,name,department,canbeadd from users");
	}
	catch(CDBException e)
	{
		AfxMessageBox(e.m_strError);
		return;
	}
	
	CDBVariant id,photoid,beadd;
	int index=0;
	while(!recordset.IsEOF())
	{		
		recordset.GetFieldValue(short(0),id);
		index=id.m_lVal-UserIdRadix;
		recordset.GetFieldValue(1,photoid);
		recordset.GetFieldValue(2,m_pUsers[index].Name);
		recordset.GetFieldValue(3,m_pUsers[index].Department);
		recordset.GetFieldValue(4,beadd);		
		m_pUsers[index].Id=id.m_lVal;
		m_pUsers[index].PhotoId=photoid.m_lVal;
		m_pUsers[index].CanbeAdd=beadd.m_chVal;
		recordset.MoveNext();
	}
	recordset.Close();

	if(!m_Socket.Create(m_nSendNum,m_anPort))
	{
		AfxMessageBox("Create Socket Error!");
		return;
	}
	
	AfxBeginThread(CheckOnline,0);
	m_bServerStarted=TRUE;	
}

void CServerApp::OnUpdateStartServer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bServerStarted);		
}

void CServerApp::OnUserOnline() 
{
	
}

void CServerApp::OnCloseServer() 
{
	m_bServerStarted=FALSE;
	m_nNumberOnline=0;
	m_nMaxUserId=0;
	m_Socket.CloseListenSocket();
	Sleep(1000);
	m_Socket.CloseSendSocket();
	m_Database.Close();	
	if(m_pUsers)
		delete[] m_pUsers;
	m_pUsers=NULL;
}

void CServerApp::OnUpdateCloseServer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bServerStarted);		
}

void CServerApp::OnAppExit() 
{
	if(m_bServerStarted)
	{
		if(AfxMessageBox("服务器正处在服务状态，您真要的关闭服务并退出吗？",MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			OnCloseServer();
		}
		else
			return;
	}
	
	//Save Server setup for client user
	FILE * file;
	file=fopen("Server.ini","w");
	fprintf(file,"IP=%s\nPort=",m_strLocalIP);	
	fprintf(file,"%d",m_anPort.GetAt(0));	
	for(int i=1;i<m_anPort.GetSize();i++)
		fprintf(file,",%d",m_anPort.GetAt(i));	
	fclose(file);
	
	CWinApp::OnAppExit();
}

void CServerApp::OnUpdateUserOnline(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bServerStarted);	
}

BOOL CServerApp::LoadMyProfileSettings()
{
	CString cfile=SETUPFILE;
	//GetLocal IP Address
	// m_strLocalIP=?;
	char rtemp[50];
	GetPrivateProfileString("数据库", "DSN",DEFAULT_DSN,rtemp,50,cfile);
	m_strDataSource=rtemp;
	GetPrivateProfileString("数据库", "UID",DEFAULT_UID,rtemp,50,cfile);
	m_strDataSUID=rtemp;
	GetPrivateProfileString("数据库", "PWD",DEFAULT_DATAS_PWD,rtemp,50,cfile);
	m_strDataSPwd=rtemp;
	m_nSendNum=::GetPrivateProfileInt("网络设置","发送端口数",DEFAULT_SEND_NUM,cfile);
	GetPrivateProfileString("网络设置","发送广播密码",DEFAULT_BROADCAST_PWD,rtemp,50,cfile);
	m_strBroadcastPwd=rtemp;
		
	char sztemp[20];	
	GetPrivateProfileString("网络设置","接受端口",DEFAULT_RECV_PORT_STR,rtemp,50,cfile);	
	int j=0;
	for(int i=0;rtemp[i];i++)
	{		
		if(rtemp[i]==',')
		{
			sztemp[j]='\0';
			m_anPort.Add(atol(sztemp));
			j=0;
		}
		else
			sztemp[j++]=rtemp[i];
	}
	sztemp[j]='\0';
	m_anPort.Add(atol(sztemp));
	m_sConnectString.Format("DSN=%s;UID=%s;PWD=%s;",m_strDataSource,m_strDataSUID,m_strDataSPwd);

	return TRUE;
}

void CServerApp::SaveMyProfileSettings()
{
	CString strPort,strtemp;	
	CString cfile=SETUPFILE;
	for(int i=0;i<m_anPort.GetSize();i++)
	{
		if(i==0)
			strtemp.Format("%d",m_anPort.GetAt(i));
		else
			strtemp.Format(",%d",m_anPort.GetAt(i));
		strPort+=strtemp;
	}
	CString tt;
	tt.Format("%d",m_nSendNum);
	WritePrivateProfileString("数据库", "DSN",m_strDataSource,cfile);
	WritePrivateProfileString("数据库", "UID",m_strDataSUID,cfile);
	WritePrivateProfileString("数据库", "PWD",m_strDataSPwd,cfile);
	WritePrivateProfileString("网络设置","发送端口数",tt,cfile);
	WritePrivateProfileString("网络设置","发送广播密码",m_strBroadcastPwd,cfile);	
	WritePrivateProfileString("网络设置","接受端口",strPort,cfile);
}
