// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Server.h"

#include "MainFrm.h"
#include "msg.h"
#include "socket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_MY_TRAY_NOTIFICATION WM_USER+101
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_NUMBER_ONLINE, OnUpdateNumberOnline)
	ON_UPDATE_COMMAND_UI(ID_NUMBER_ALLUSER, OnUpdateNumberAlluser)
	ON_UPDATE_COMMAND_UI(ID_NUMBER_RECV, OnUpdateNumberRecvReq)
	ON_COMMAND(ID_SHOWWINDOW, OnShowwindow)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MY_TRAY_NOTIFICATION,OnTrayNotification)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_NUMBER_ONLINE,
	ID_NUMBER_ALLUSER,
	ID_NUMBER_RECV,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
		
	m_TrayIcon.SetNotificationWnd(this,WM_MY_TRAY_NOTIFICATION);
	m_TrayIcon.SetIcon(IDR_TRAYICON,"服务器");

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


void CMainFrame::OnUpdateNumberOnline(CCmdUI* pCmdUI) 
{
	CString str;	
	str.Format("在线人数：  %d",((CServerApp*)AfxGetApp())->m_nNumberOnline);
	pCmdUI->SetText(str);	
}

void CMainFrame::OnUpdateNumberAlluser(CCmdUI* pCmdUI) 
{
	CString str;	
	str.Format("总用户数：  %d",((CServerApp*)AfxGetApp())->m_nTotalUserNumber);
	pCmdUI->SetText(str);
}

void CMainFrame::OnUpdateNumberRecvReq(CCmdUI* pCmdUI) 
{
	CString str;	
	str.Format("接受消息：  %d",((CServerApp*)AfxGetApp())->m_nRecvMsg);
	pCmdUI->SetText(str);	
}

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_SYSCOMMAND)
	{
		if(wParam==SC_CLOSE)
		{
			PostMessage(WM_COMMAND,ID_APP_EXIT,0);
			return 0;
		}
		else if(wParam==SC_MINIMIZE)
		{
			PostMessage(WM_COMMAND,ID_SHOWWINDOW,0);
			return 0;
		}
	}

	return CFrameWnd::DefWindowProc(message, wParam, lParam);
}


void CMainFrame::OnTest() 
{
	
}

LRESULT CMainFrame::OnTrayNotification(WPARAM wp,LPARAM lp)
{
	return m_TrayIcon.OnTrayNotification(wp,lp);
}

void CMainFrame::OnShowwindow() 
{
	static BOOL bShow=FALSE;
	if(bShow)ShowWindow(SW_NORMAL);
	else ShowWindow(SW_HIDE);
	bShow=!bShow;
}
