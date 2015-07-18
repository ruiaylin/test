// Client.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Client.h"

#include "MainFrm.h"
#include "LoginDlg.h"
#include "registerdlg.h"
#include "SetupServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinApp)
	//{{AFX_MSG_MAP(CClientApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_SYS_SETUP, OnSysSetup)
	ON_COMMAND(ID_SEND_FILE, OnSendFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientApp construction

CClientApp::CClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_uCurrentUserID=0;
	m_bOnlineState=0;
	m_nCurSysMsg=0;
	m_nCurNotice=0;	
	m_nCurOther=0;
	m_nBitmapNum=0;
	m_nNumberHeadBmp=0;
	m_nTotalOnline=0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClientApp object

CClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CClientApp initialization

BOOL CClientApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

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
	SetRegistryKey(_T("My Program"));

	if(!m_Socket.Create())
	{
		AfxMessageBox(IDS_CREATE_SOCKET_ERROR);
		return FALSE;
	}

	if(!LoadFaceBmp())
	{
		AfxMessageBox(IDS_FACE_FILE_LOAD_FAILE);
		return FALSE;
	}
	
	CArray<int,int>AllUserID;
	
	int LoginType=ONLINE;
	BOOL bApplyID=FALSE;

	if(!LoadLocalAllUserID(AllUserID))	
Regiser_begin:
	{
		CRegisterDlg dlg;
		m_Socket.SetOwner(&dlg);
		
		CStdioFile file1;	
		if(file1.Open("Server.ini",CFile::modeRead))
		{		
			CString str1,str2,str3,str4;
			file1.ReadString(str1);	
			file1.ReadString(str2);	
			file1.Close();
			m_strServerIP=str1.Mid(3);			
		    m_uServerPort=atoi(str2.GetBuffer(50)+5);			
		}
		else
		{
			m_strServerIP=DEFAULT_SERVER_IP;
			m_uServerPort=DEFAULT_SERVER_PORT;		
		}		
		
		//m_strServerIP="192.168.0.208";
		//m_uServerPort=4000;
		dlg.m_strServerIP=m_strServerIP;
		dlg.m_uServerPort=m_uServerPort;
		if(dlg.DoModal()==IDCANCEL)
		{
			m_Socket.Close();
			return FALSE;
		}
		m_uCurrentUserID=dlg.m_uUserID;
		m_strCurUserDir.Format("%d\\",m_uCurrentUserID);
		m_strCurIDpwd=dlg.m_strPassword;
		m_strServerIP=dlg.m_strServerIP;
		m_uServerIP=dlg.m_uServerIP;
		m_uServerPort=dlg.m_uServerPort;
		SaveCurIDToFile();
		::CreateDirectory(m_strCurUserDir,NULL);
		CFile f1(m_strCurUserDir+USERSETUP,CFile::modeCreate|CFile::modeWrite);
		f1.Write((char*)&m_uServerIP,sizeof(DWORD));
		f1.Write((char*)&m_uServerPort,sizeof(DWORD));
		f1.Close();
		if(dlg.m_bApplyID)
		{			
			bApplyID=TRUE;			
		}
	}
	else
	{
		CLoginDlg dlg;
		dlg.SetUserID(AllUserID);
		m_Socket.SetOwner(&dlg);
		if(dlg.DoModal()==IDCANCEL)
		{
			m_Socket.Close();
			return FALSE;
		}
		if(dlg.lresult==1)
			goto Regiser_begin;

		if(dlg.m_LoginType)LoginType=ONHIDE;
		else LoginType=ONLINE;
		m_uCurrentUserID=dlg.uCurID;
		m_strCurIDpwd=dlg.pwd;
		m_strCurUserDir.Format("%d\\",m_uCurrentUserID);
		m_uServerIP=dlg.uServerIP;
		m_uServerPort=dlg.uPort;
		LoadFriendInfo();
	}
	
	in_addr tIP;
	tIP.S_un.S_addr=m_uServerIP;
	m_strServerIP=inet_ntoa(tIP);
	
	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	
	m_Socket.SetOwner(pFrame);	
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	
	if(bApplyID)
	{		
		if(!m_bOnlineState)
			pFrame->SetTimer(1,TimerSpanClient,NULL);
		m_bOnlineState=1;
		CString str=m_strCurUserDir;			
		str=str.Left(str.GetLength()-1);
		pFrame->m_TrayIcon.SetIcon(IDR_ONLINE,str);
	}
	else
	{
		CData msg;	
		msg.index=LoginType;		
		msg.MyId=m_uCurrentUserID;
		msg.tarIP=m_uServerIP;
		msg.nPort=m_uServerPort;		
		m_Socket.SendDataDirect(&msg);		
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CClientApp message handlers

BOOL CClientApp::LoadLocalAllUserID(CArray<int,int>&aID)
{
	CFile file;
	if(!file.Open(ALLUSERIDFILE,CFile::modeRead))
		return FALSE;
	int num=0,data;
	file.Read(&num,sizeof(int));
	for(int i=0;i<num;i++)
	{
		file.Read(&data,sizeof(int));
		aID.Add(data);
	}
	file.Close();
	return TRUE;
}

BOOL CClientApp::LoadFaceBmp()
{
	FILE* file;
	int num=0;
	file=fopen(FACEINIFILE,"r");
	if(!file)return FALSE;	
	fscanf(file,"%d",&num);
	CArray<CString,CString>aFilename;
	
	char temp[50];
	for(int i=0;i<num;i++)
	{
		fscanf(file,"%s",temp);
		aFilename.Add(temp);	
	}
	fclose(file);
	m_imaLarge.Create(32,32,ILC_COLOR16,num+1,70);
	m_imaSmall.Create(16,16,ILC_COLOR16,num+1,70);
	m_imaHead.Create(32,32,ILC_COLOR16,num/2+1,70);
	m_imaSmallHead.Create(16,16,ILC_COLOR16,num/2+1,70);

	m_nBitmapNum=num;
	m_nNumberHeadBmp=num/2;
	m_paBitmap=new CBitmap*[num];
	m_paSmallBitmap=new CBitmap*[num];
	
	memset(m_paBitmap,0,sizeof(CBitmap*)*num);
	memset(m_paSmallBitmap,0,sizeof(CBitmap*)*num);
	HBITMAP bitmap,bit;	
	for(i=0;i<num;i++)
	{
		CString str="face\\"+aFilename.GetAt(i);
		bitmap=(HBITMAP)LoadImage(AfxGetInstanceHandle(),str,
			IMAGE_BITMAP,0,0,
			LR_LOADFROMFILE|LR_CREATEDIBSECTION);
		m_paBitmap[i]=new CBitmap;
		m_paSmallBitmap[i]=new CBitmap;
		m_paBitmap[i]->Attach(bitmap);		
		bit=(HBITMAP)::CopyImage(bitmap,IMAGE_BITMAP,16,16,LR_CREATEDIBSECTION);
		m_paSmallBitmap[i]->Attach(bit);
		
		m_imaLarge.Add(m_paBitmap[i],crColorMask);
		m_imaSmall.Add(m_paSmallBitmap[i],crColorMask);		
		if(i%2==0)
		{
			m_imaHead.Add(m_paBitmap[i],crColorMask);
			m_imaSmallHead.Add(m_paSmallBitmap[i],crColorMask);
		}
	}

	aFilename.RemoveAll();

	return TRUE;
}

BOOL CClientApp::LoadFriendInfo()
{
	CFile file;
	int bufoffset=UserFileReservedLength+MaxPasswordLength+3;
	int countoffset=UserFileReservedLength+1;
	if(!file.Open(m_strCurUserDir+USERFILE,CFile::modeRead))
	{
		return FALSE;
	}
	
	WORD uCount=0;
	file.Seek(countoffset,CFile::begin);
	file.Read(&uCount,sizeof(WORD));

	file.Seek(bufoffset,CFile::begin);
	for(int i=0;i<uCount;i++)
	{
		UserInfo * pInfo=new UserInfo;
		file.Read(pInfo,sizeof(UserInfo));
		m_aUserInfo.Add(pInfo);
		FriendState* pState=new FriendState;
		m_aUserState.Add(pState);		
	}
	
	file.Close();
	return TRUE;
}

BOOL CClientApp::SaveUserInfo()
{
	CFile file;

	if(!file.Open(m_strCurUserDir+USERFILE,CFile::modeCreate|CFile::modeWrite))
	{
	//	AfxMessageBox(IDS_CREATE_USERFILE_FAIL);
		return FALSE;
	}
	char * pRand=new char[UserFileReservedLength];
	file.Write(pRand,UserFileReservedLength);
	UCHAR ch;
	ch=m_strCurIDpwd.GetLength();
	//  jia mi this pwd
	char* pPassword=new char[MaxPasswordLength+3];
	pPassword[0]=ch;
	int nFriendCount=m_aUserInfo.GetSize();
	StrNCopy(pPassword+1,(char*)&nFriendCount,2);
	StrNCopy(pPassword+3,m_strCurIDpwd.GetBuffer(ch),ch);
	file.Write(pPassword,MaxPasswordLength+3);
	for(int i=0;i<m_aUserInfo.GetSize();i++)
		file.Write(m_aUserInfo.GetAt(i),sizeof(UserInfo));

	file.Close();
	delete pRand;
	delete pPassword;
	return TRUE;
}

void CClientApp::SaveCurIDToFile()
{
	CFile file;
	int num=0;
	if(!file.Open(ALLUSERIDFILE,CFile::modeReadWrite))
	{
		file.Open(ALLUSERIDFILE,CFile::modeCreate|CFile::modeWrite);	
	}
	else
	{
		file.Read(&num,sizeof(int));
	}
	num++;
	file.Seek(0,CFile::begin);
	file.Write(&num,sizeof(int));
	file.Seek(0,CFile::end);
	file.Write(&m_uCurrentUserID,sizeof(int));
	file.Close();
}

int CClientApp::ExitInstance() 
{		
	if(m_bOnlineState)
	{
		CData msg;
		msg.index=OFFLINE;
		msg.MyId=m_uCurrentUserID;
		msg.tarIP=m_uServerIP;
		msg.nPort=m_uServerPort;
		m_Socket.SendDataDirect(&msg);
	}	
	
	//---------- save user info --------------	

	if(m_uCurrentUserID!=0)
		SaveUserInfo();

	//----- destroy -------
	for(int i=0;i<m_aUserInfo.GetSize();i++)
	{
		delete m_aUserInfo.GetAt(i);
		FriendState *pState=m_aUserState.GetAt(i);	
		delete pState;
	}
	for(i=0;i<m_aStranger.GetSize();i++)
	{
		delete m_aStranger.GetAt(i);		
		FriendState * pState=m_aStrangerState.GetAt(i);				
		delete pState;
	}		
	for(i=0;i<m_nBitmapNum;i++)
	{
		if(m_paBitmap[i]!=NULL)delete m_paBitmap[i];
		if(m_paSmallBitmap[i]!=NULL)delete m_paSmallBitmap[i];
	}
	delete[m_nBitmapNum]m_paBitmap;
	delete[m_nBitmapNum]m_paSmallBitmap;
	m_imaLarge.DeleteImageList();
	m_imaSmall.DeleteImageList();
	m_imaHead.DeleteImageList();
	m_imaSmallHead.DeleteImageList();
	return CWinApp::ExitInstance();
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
void CClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CClientApp::OnSysSetup() 
{	
	static CString strServerIP=m_strServerIP;
	static DWORD uServerPort=m_uServerPort;
	CSetupServerDlg dlg;
	dlg.m_port=uServerPort;
	dlg.m_strServerIP=strServerIP;	
	if(dlg.DoModal()==IDOK)
	{	
		if(!(dlg.m_strServerIP==strServerIP&&dlg.uPort==uServerPort))
		{	
			strServerIP=dlg.m_strServerIP;
			uServerPort=dlg.uPort;
			CFile file;
			if(file.Open(m_strCurUserDir+USERSETUP,CFile::modeCreate|CFile::modeWrite))
			{		
				file.Write((char*)&dlg.uServerIP,sizeof(DWORD));
				file.Write((char*)&dlg.uPort,sizeof(DWORD));
				file.Close();
				AfxMessageBox(IDS_SETUP_NOTICE);
			}
		}
	}	
}

void CClientApp::OnSendFile() 
{
	AfxMessageBox(IDS_NOT_FINISH);
}

