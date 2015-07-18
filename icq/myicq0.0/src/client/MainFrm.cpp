// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "msg.h"
#include "Client.h"
#include "lookdlg.h"
#include "talkdlg.h"
#include "frienddetail.h"
#include "modifypidlg.h"
#include "searchdlg.h"
#include "SendToalldlg.h"
#include "multisenddlg.h"
#include "ShowOnlinedlg.h"
#include "SendBroadcastDlg.h"
#include "ShowAddMsgDlg.h"
#include "ShowBroadcastDlg.h"
#include "addfrienddlg.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND(ID_SEND_MESSAGE, OnSendMessage)
	ON_UPDATE_COMMAND_UI(ID_SEND_MESSAGE, OnUpdateSendMessage)
	ON_COMMAND(ID_SEND_EMAIL, OnSendEmail)
	ON_UPDATE_COMMAND_UI(ID_SEND_EMAIL, OnUpdateSendEmail)
	ON_COMMAND(ID_FRIEND_DETAIL, OnFriendDetail)
	ON_COMMAND(ID_PERSON_HOMEPAGE, OnPersonHomepage)
	ON_UPDATE_COMMAND_UI(ID_PERSON_HOMEPAGE, OnUpdatePersonHomepage)
	ON_COMMAND(ID_OFFLINE, OnOffline)
	ON_COMMAND(ID_ONHIDE, OnOnhide)
	ON_COMMAND(ID_ONLINE, OnOnline)
	ON_COMMAND(ID_SEARCH, OnSearch)
	ON_COMMAND(ID_PERSONINFO_SETUP, OnPersoninfoSetup)
	ON_COMMAND(ID_MULTISEND, OnMultisend)
	ON_COMMAND(ID_SENDTO_ALL, OnSendtoAll)
	ON_COMMAND(ID_MESSAGE_MANAGE, OnMessageManage)
	ON_COMMAND(ID_SHOW_ONLINE, OnShowOnline)
	ON_COMMAND(ID_SEND_BROADCAST, OnSendBroadcast)
	ON_COMMAND(ID_ADD_FRIEND, OnAddFriend)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MY_TRAY_NOTIFICATION,OnTrayNotification)
	ON_MESSAGE(WM_HOTKEY,OnHotKey)
	ON_MESSAGE(WM_OUTBAR_NOTIFY, OnOutbarNotify)
	ON_MESSAGE(WM_RECIEVE_MSG,ProcRecv)
	ON_MESSAGE(WM_ADDFRIEND_REFRESH,OnAddFriendRefresh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pSearch=NULL;	
	m_pModifyPI=NULL;
	m_pMultiSend=NULL;
	m_pSendToAll=NULL;		
	m_pShowOnline=NULL;
	m_pSendBroad=NULL;	
	m_pShowNotice=NULL;
	m_pAddFriendDlg=NULL;
	m_bRecvTotal=FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;	
			
	DWORD dwf = CGfxOutBarCtrl::fDragItems|CGfxOutBarCtrl::fEditGroups|CGfxOutBarCtrl::fEditItems|CGfxOutBarCtrl::fRemoveGroups|
		CGfxOutBarCtrl::fRemoveItems|CGfxOutBarCtrl::fAddGroups|CGfxOutBarCtrl::fAnimation;	
		
	m_wndBar.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this,AFX_IDW_PANE_FIRST, dwf);
	m_wndBar.SetOwner(this);	
	
	m_wndBar.SetImageList(&GetApp()->m_imaLarge, CGfxOutBarCtrl::fLargeIcon);
	m_wndBar.SetImageList(&GetApp()->m_imaSmall, CGfxOutBarCtrl::fSmallIcon);
	m_wndBar.AddFolder("我的好友", 0);	
	m_wndBar.AddFolder("陌生人", 1);
	m_wndBar.AddFolder("黑名单", 2);
	
	UserInfo* pInfo;
	CString str;
	for(int i=1;i<GetApp()->m_aUserInfo.GetSize();i++)
	{
		pInfo=GetApp()->m_aUserInfo.GetAt(i);
		if(pInfo->HaveDetail)
			m_wndBar.AddItem(0,pInfo->Name,pInfo->PhotoId,pInfo->Id);
		else
		{
			str.Format("%d",pInfo->Id);
			m_wndBar.AddItem(0,str,0,pInfo->Id);
		}
	}
	m_wndBar.SetSelFolder(0);

	BOOL result1=::RegisterHotKey(m_hWnd,1001,MOD_CONTROL|MOD_ALT,'X');
	BOOL result2=::RegisterHotKey(m_hWnd,1002,MOD_CONTROL|MOD_ALT,'x');
	BOOL result3=::RegisterHotKey(m_hWnd,1003,MOD_CONTROL|MOD_ALT,'m');
	BOOL result4=::RegisterHotKey(m_hWnd,1004,MOD_CONTROL|MOD_ALT,'M');
	if(!result1&&!result2&&!result3&&!result4)
	{
		//AfxMessageBox(IDS_HOTKEY_REG_FAIL);
	}	

	m_TrayIcon.SetNotificationWnd(this,WM_MY_TRAY_NOTIFICATION);
	str=GetApp()->m_strCurUserDir;
	str=str.Left(str.GetLength()-1);
	m_TrayIcon.SetIcon(IDR_OFFLINE,str);	

	str.Format("ID:%d",GetApp()->m_uCurrentUserID);
	SetWindowText(str);
	CMenu* pMenu=GetMenu();
	menu1.Attach(pMenu->GetSubMenu(0)->GetSafeHmenu());
	menu2.Attach(pMenu->GetSubMenu(1)->GetSafeHmenu());
	menu1.LoadToolBarResource(IDR_TOOLBAR1);
	menu2.LoadToolBarResource(IDR_TOOLBAR2);
	menu1.RemapMenu(&menu1);
	menu2.RemapMenu(&menu2);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	
	cs.dwExStyle|=WS_EX_TOOLWINDOW|WS_EX_TOPMOST;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.x=650;
	cs.y=50;
	cs.cx=90;
	cs.cy=500;
	return TRUE;
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

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndBar.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndBar.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = 90;
	lpMMI->ptMaxTrackSize.x=200;
	lpMMI->ptMinTrackSize.y=200;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

LRESULT CMainFrame::OnHotKey(WPARAM wParam,LPARAM lParam)
{
	if (wParam== 1001||wParam==1002)
	{
		ShowWindow(SW_NORMAL); 
		SetForegroundWindow();
		return 1;
	}
	else if(wParam== 1003||wParam==1004)
	{
		SendMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
		return 1;
	}
	return 0;
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_SYSCOMMAND)
	{
		if(wParam==SC_MINIMIZE)
		{
			ShowWindow(SW_HIDE);
			return 0;
		}
		else if(wParam==SC_CLOSE)
		{
			ShowWindow(SW_HIDE);
			return 0;
		}
	}
			
	return CFrameWnd::WindowProc(message, wParam, lParam);	
}

LRESULT CMainFrame::OnTrayNotification(WPARAM wp,LPARAM lp)
{
	return m_TrayIcon.OnTrayNotification(wp,lp);
}

void CMainFrame::OnClose() 
{
	::UnregisterHotKey(m_hWnd,1001); 
	::UnregisterHotKey(m_hWnd,1002); 
	::UnregisterHotKey(m_hWnd,1003); 
	::UnregisterHotKey(m_hWnd,1004); 
	CFrameWnd::OnClose();
}

LRESULT CMainFrame::OnOutbarNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case NM_OB_ITEMCLICK:
		// cast the lParam to an integer to get the clicked item
			{
				nGroupFlag=m_wndBar.iSelFolder;				
				nCurSelID = m_wndBar.GetItemData(lParam);
				nCurSelIndex=lParam;
				CMenu menu;
				CGfxPopupMenu cMenu;
				menu.LoadMenu(IDR_POPMENU);
				cMenu.Attach(menu.GetSubMenu(0)->GetSafeHmenu());
				if(nGroupFlag==1)
				{
					cMenu.AppendMenu(MF_STRING,ID_ADD_FRIEND,"加为好友");
				}				
				cMenu.LoadToolBarResource(IDR_TOOLBAR3);
				cMenu.RemapMenu(&cMenu);				
				CPoint mouse;
				GetCursorPos(&mouse);				
				cMenu.TrackPopupMenu(TPM_LEFTALIGN,mouse.x,mouse.y,this);						
			}		
			return 1;
		case NM_OB_ITEMDCLICK:
			{
				nGroupFlag=m_wndBar.iSelFolder;				
				nCurSelID = m_wndBar.GetItemData(lParam);
				nCurSelIndex=lParam;
				SendMessage(WM_COMMAND,ID_SEND_MESSAGE,0);				
			}
			return 1;
		case NM_OB_ONLABELENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited item
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				//TRACE2("Editing item %d, new text:%s\n", pOI->index, pOI->cText);
			}
		return 1;

		case NM_OB_ONGROUPENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited folder
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				TRACE2("Editing folder %d, new text:%s\n", pOI->index, pOI->cText);
			}
		return 1;

		case NM_OB_DRAGITEM:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the dragged items
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				TRACE2("Drag item %d at position %d\n", pOI->iDragFrom, pOI->iDragTo);
			}
		return 1;
	}
	return 0;
}

LRESULT CMainFrame::ProcRecv(WPARAM wParam,LPARAM lParam)
{
	CData* pData=(CData*)wParam;
	UINT index=pData->index;
	int & iOnlineState=GetApp()->m_bOnlineState;

	switch(index)
	{
	case SEND_MSG_TO_FRIEND:
		{
			//recv from friend or server
			if(iOnlineState==0)break;
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())
			{
				TRACE0("msg.LoadFromBuf error!\n");
				break;
			}

			int mindex=0;
			SaveMsg* pMsg=new SaveMsg;
			pMsg->type=NORMAL_MSG;
			pMsg->ID=msg.MyId;
			pMsg->Time=msg.Time;
			pMsg->Msg=msg.Msg;

			UserInfo *pInfo=NULL;
			FriendState *pState=NULL;
			if(FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,msg.MyId,pInfo,pState))
			{
				pState->aMsg.Add(pMsg);			
			}			
			else
			{
				// 陌生人			
				if(FindInArray(GetApp()->m_aStranger,GetApp()->m_aStrangerState,msg.MyId,pInfo,pState))
				{
					pState->aMsg.Add(pMsg);
				}
				else
				{
					pInfo=new UserInfo;					
					pInfo->Id=msg.MyId;
					pInfo->HaveDetail=0;
					pState=new FriendState;					
					pState->IP=msg.tarIP;
					pState->Port=msg.nPort;
					pState->OnlineState=1;
					pState->aMsg.Add(pMsg);
					GetApp()->m_aStranger.Add(pInfo);					
					GetApp()->m_aStrangerState.Add(pState);
					GetApp()->m_aStranger.GetSize();
					CString str;
					str.Format("%d",pInfo->Id);
					m_wndBar.AddItem(1,str,0,pInfo->Id);					
					m_wndBar.Invalidate();
				}
			}
			if(pState->pRecv==NULL)
			{
				pState->pRecv=new CLookDlg;
				pState->pRecv->Create(CLookDlg::IDD,GetDesktopWindow());
			}
			pState->pRecv->PostMessage(WM_RECVMSG,(WPARAM)pInfo,(LPARAM)pState);			
			
			break;
		}
	case SOMEONE_ONHIDE:
		{				
			if(iOnlineState==0)break;

			CNoticeMsg msg;
			CopyDataBuf(&msg,pData);

			if(!msg.LoadFromBuf())break;

			UserInfo *pInfo=NULL;
			FriendState *pState=NULL;
			if(FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,msg.MyId,pInfo,pState))
			{
				pState->OnlineState=2;
				pState->IP=msg.IP;
				pState->Port=msg.Port;
				m_wndBar.SetOnlineImage(msg.MyId,FALSE);
				m_wndBar.Invalidate();
			}			
			break;
		}
	case SOMEONE_ONLINE:
		{
			if(iOnlineState==0)break;

			CNoticeMsg msg;
			CopyDataBuf(&msg,pData);

			if(!msg.LoadFromBuf())break;

			UserInfo *pInfo=NULL;
			FriendState *pState=NULL;
			if(FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,msg.MyId,pInfo,pState))
			{
				pState->OnlineState=1;
				pState->IP=msg.IP;
				pState->Port=msg.Port;
				m_wndBar.SetOnlineImage(msg.MyId);
				m_wndBar.Invalidate();
			}						
			break;
		}
	case SOMEONE_OFFLINE:
		{
			if(iOnlineState==0)break;
			UserInfo *pInfo=NULL;
			FriendState *pState=NULL;
			if(FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,pData->MyId,pInfo,pState))
			{
				pState->OnlineState=0;				
				m_wndBar.SetOnlineImage(pData->MyId,FALSE);	
				m_wndBar.Invalidate();
			}			
			break;
		}
	case ONLINE_OK:
		{
			if(!GetApp()->m_bOnlineState)
				SetTimer(1,TimerSpanClient,NULL);
			GetApp()->m_bOnlineState=1;
			CString str=GetApp()->m_strCurUserDir;			
			str=str.Left(str.GetLength()-1);
			m_TrayIcon.SetIcon(IDR_ONLINE,str);			
			AfxBeginThread(RequestFriendDetail,0,THREAD_PRIORITY_BELOW_NORMAL);			
			break;
		}
	case ONHIDE_OK:
		{
			if(!GetApp()->m_bOnlineState)
				SetTimer(1,TimerSpanClient,NULL);
			GetApp()->m_bOnlineState=2;
			CString str=GetApp()->m_strCurUserDir;				
			str=str.Left(str.GetLength()-1);
			m_TrayIcon.SetIcon(IDR_ONHIDE,str);
			AfxBeginThread(RequestFriendDetail,0,THREAD_PRIORITY_BELOW_NORMAL);			
			break;
		}
	case ALL_FRIEND_ID:
		{
			if(iOnlineState==0)break;
			CMsg2 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())
				break;			
			
			CString str1;
			for(int i=0;i<msg.aFriendId.GetSize();i++)
			{
				if(FindInArray(GetApp()->m_aUserInfo,msg.aFriendId.GetAt(i))!=-1)
					continue;

				UserInfo* pInfo=new UserInfo;				
				pInfo->Id=msg.aFriendId.GetAt(i);
				str1.Format("%d",pInfo->Id);
				pInfo->HaveDetail=0;
				lstrcpy(pInfo->Name,str1);
				FriendState* pState=new FriendState;					
				pState->OnlineState=0;
				GetApp()->m_aUserInfo.Add(pInfo);					
				GetApp()->m_aUserState.Add(pState);
				GetApp()->m_aUserInfo.GetSize();				
				
				m_wndBar.AddItem(0,str1,0,pInfo->Id);
			}		
			m_wndBar.Invalidate();
			break;
		}
	case ONLINE_FRIEND:		
		{
			//current all online friend
			if(iOnlineState==0)break;
			CMsgOnlineFriend msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			UserInfo *pInfo=NULL;
			FriendState *pState=NULL;
			for(int i=0;i<msg.aFriendId.GetSize();i++)
			{				
				if(FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,msg.aFriendId.GetAt(i),pInfo,pState))
				{
					pState->OnlineState=msg.aFriendState.GetAt(i);
					pState->IP=msg.aFriendIP.GetAt(i);
					pState->Port=msg.aFriendPort.GetAt(i);
					if(pState->OnlineState==1)
						m_wndBar.SetOnlineImage(msg.aFriendId.GetAt(i));
				}
			}
			m_wndBar.Invalidate();			
			break;
		}
	case RE_FRIEND_DETAIL:
		{
			if(iOnlineState==0)break;
			CMsgPerson msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;

			TRACE1("Friend Detail %d\n",msg.MyId);
			UserInfo *pInfo=NULL;			
			if(FindInArray(GetApp()->m_aUserInfo,pData->MyId,pInfo))
			{
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
				m_wndBar.ModifyItem(pInfo->Id,pInfo->PhotoId,pInfo->Name);
				m_wndBar.Invalidate();
			}
			else if(FindInArray(GetApp()->m_aStranger,pData->MyId,pInfo))
			{
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
				m_wndBar.ModifyItem(pInfo->Id,pInfo->PhotoId,pInfo->Name);
				m_wndBar.Invalidate();
			}			
			break;
		}
	case BE_ADDED_AS_FRIEND:
	case FRIEND_IDENTITY_VALIDATE:
		{			
			if(iOnlineState==0)break;
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			SaveMsg * pMsg=new SaveMsg;
			if(index==BE_ADDED_AS_FRIEND)pMsg->type=BEADDED_MSG;
			else pMsg->type=ID_CHECK_MSG;

			pMsg->ID=msg.MyId;
			pMsg->Time=msg.Time;
			pMsg->Msg=msg.Msg;
			GetApp()->m_aSysMsg.Add(pMsg);
			GetApp()->m_aOtherMsg.Add(pMsg);

			if(m_pShowOther==NULL)
			{
				m_pShowOther=new CShowAddMsgDlg;
				m_pShowOther->Create(CShowAddMsgDlg::IDD,GetDesktopWindow());
			}
			//m_pShowOther->PostMessage();
			
			
			TRACE0("R: BE_ADDED_AS_FRIEND\n");
			break;
		}
	case SYSTEM_BROADCAST:
	case ACCEPT_AS_FRIEND:
	case REFUSE_AS_FRIEND:
		{
			if(iOnlineState==0)break;
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			SaveMsg * pMsg=new SaveMsg;
			pMsg->type=NOTICE_MSG;
			pMsg->ID=msg.MyId;
			pMsg->Time=msg.Time;
			pMsg->Msg=msg.Msg;
			GetApp()->m_aSysMsg.Add(pMsg);
			GetApp()->m_aNoticeMsg.Add(pMsg);
			
			if(m_pShowNotice==NULL)
			{
				m_pShowNotice=new CShowBroadcastDlg;
				m_pShowNotice->Create(CShowBroadcastDlg::IDD,GetDesktopWindow());
			}
			m_pShowNotice->PostMessage(WM_SHOW_NOTICE);
			
			break;
		}
	case TOTAL_ONLINE:
		{
			if(iOnlineState==0)break;
			CMsg3 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			if(msg.Value>0)
			{
				GetApp()->m_nTotalOnline=msg.Value;
				CString str;
				str.Format("%d人在线",msg.Value);
				SetWindowText(str);
			}
			m_bRecvTotal=TRUE;
			TRACE0("R: TOTAL_ONLINE\n");
			break;
		}	
	case NAME_NOT_FOUND_BY_NAME:
		{			
			if(iOnlineState==0)break;
			break;
		}	
	case FOUND_FRIEND_BY_ID:
		{
			if(iOnlineState==0)break;
			CMsgPerson msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			for(int i=0;i<m_aTempInfo.GetSize();i++)
				delete m_aTempInfo.GetAt(i);
			m_aTempInfo.RemoveAll();
			
			UserInfo *pInfo=new UserInfo;
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
			m_aTempInfo.Add(pInfo);
			
			break;
		}
	case ID_NOT_FOUND_BY_ID:
		{
			if(iOnlineState==0)break;
			
			break;
		}	
	case RE_ADD_AS_FRIEND:
		{
			if(iOnlineState==0)break;
			CMsg3 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;


			
			break;
		}		
	}

	if(pData)
		delete pData;	
	return 0;
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1&&GetApp()->m_bOnlineState)
	{
/*		CString str;
		if(m_bRecvTotal)
		{
			m_bRecvTotal=FALSE;			
		}
		else
		{
			GetApp()->m_bOnlineState=0;					
			str.Format("%d",GetApp()->m_uCurrentUserID);		
			m_TrayIcon.SetIcon(IDR_OFFLINE,str);
			for(int i=0;i<GetApp()->m_aUserState.GetSize();i++)
				GetApp()->m_aUserState.GetAt(i)->OnlineState=0;
			m_wndBar.SetOfflineState();			
			KillTimer(1);
		}		
		str.Format("ID:%d",GetApp()->m_uCurrentUserID);
		SetWindowText(str);		
*/	}	
}

void CMainFrame::OnSendMessage() 
{	
	UserInfo *pInfo=NULL;
	FriendState *pState=NULL;
	if(nGroupFlag==0||nGroupFlag>2)
	{
		FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,nCurSelID,pInfo,pState);	
	}
	else
	{
		FindInArray(GetApp()->m_aStranger,GetApp()->m_aStrangerState,nCurSelID,pInfo,pState);
	}
	if(pState->pTalk==NULL)
	{
		pState->pTalk=new CTalkDlg;
		pState->pTalk->Create(CTalkDlg::IDD,GetDesktopWindow());
	}
	pState->pTalk->SendMessage(WM_SEND_MSG,(WPARAM)pInfo,(LPARAM)pState);	
}

void CMainFrame::OnUpdateSendMessage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetApp()->m_bOnlineState);
}

void CMainFrame::OnSendEmail() 
{
	UserInfo *pInfo=NULL;
	FriendState *pState=NULL;
	if(nGroupFlag==0||nGroupFlag>2)
	{
		FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,nCurSelID,pInfo,pState);	
	}
	else
	{
		FindInArray(GetApp()->m_aStranger,GetApp()->m_aStrangerState,nCurSelID,pInfo,pState);
	}
	CString str(pInfo->Email);
	ShellExecute(NULL,"open","mailto:"+str,NULL,NULL,SW_SHOW);	
}

void CMainFrame::OnUpdateSendEmail(CCmdUI* pCmdUI) 
{
	UserInfo *pInfo=NULL;
	FriendState *pState=NULL;
	if(nGroupFlag==0||nGroupFlag>2)
	{
		FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,nCurSelID,pInfo,pState);	
	}
	else
	{
		FindInArray(GetApp()->m_aStranger,GetApp()->m_aStrangerState,nCurSelID,pInfo,pState);
	}
	
	BOOL enabled=pInfo->HaveDetail==1&&lstrlen(pInfo->Email)>0;
	pCmdUI->Enable(enabled);
}

void CMainFrame::OnFriendDetail() 
{
	UserInfo *pInfo=NULL;
	FriendState *pState=NULL;
	if(nGroupFlag==0||nGroupFlag>2)
	{
		FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,nCurSelID,pInfo,pState);	
	}
	else
	{
		FindInArray(GetApp()->m_aStranger,GetApp()->m_aStrangerState,nCurSelID,pInfo,pState);
	}
	if(pState->pFInfo==NULL)
	{
		pState->pFInfo=new CFriendDetailDlg;
		pState->pFInfo->Create(CFriendDetailDlg::IDD,GetDesktopWindow());
	}
	pState->pFInfo->SendMessage(WM_SHOW_FRIEND_DETAIL,(WPARAM)pInfo,(LPARAM)pState);	
}

void CMainFrame::OnPersonHomepage() 
{		
	UserInfo *pInfo=NULL;
	FriendState *pState=NULL;
	if(nGroupFlag==0||nGroupFlag>2)
	{
		FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,nCurSelID,pInfo,pState);	
	}
	else
	{
		FindInArray(GetApp()->m_aStranger,GetApp()->m_aStrangerState,nCurSelID,pInfo,pState);
	}
	CString str(pInfo->Homepage);
	ShellExecute(NULL,"open","http://"+str,NULL,NULL,SW_SHOW);		
}

void CMainFrame::OnUpdatePersonHomepage(CCmdUI* pCmdUI) 
{
	UserInfo *pInfo=NULL;
	FriendState *pState=NULL;
	if(nGroupFlag==0||nGroupFlag>2)
	{
		FindInArray(GetApp()->m_aUserInfo,GetApp()->m_aUserState,nCurSelID,pInfo,pState);	
	}
	else
	{
		FindInArray(GetApp()->m_aStranger,GetApp()->m_aStrangerState,nCurSelID,pInfo,pState);
	}
	
	BOOL enabled=pInfo->HaveDetail==1&&lstrlen(pInfo->Homepage)>0;
	pCmdUI->Enable(enabled);
}

void CMainFrame::OnOffline() 
{
 	if(GetApp()->m_bOnlineState)
	{
		CData msg;
		msg.index=OFFLINE;
		msg.MyId=GetApp()->m_uCurrentUserID;
		msg.tarIP=GetApp()->m_uServerIP;
		msg.nPort=GetApp()->m_uServerPort;

		GetApp()->m_Socket.SendDataDirect(&msg);
		GetApp()->m_bOnlineState=0;
		CString str;
		str.Format("%d",GetApp()->m_uCurrentUserID);		
		m_TrayIcon.SetIcon(IDR_OFFLINE,str);
		for(int i=0;i<GetApp()->m_aUserState.GetSize();i++)
			GetApp()->m_aUserState.GetAt(i)->OnlineState=0;
		m_wndBar.SetOfflineState();
		KillTimer(1);
		str.Format("ID:%d",GetApp()->m_uCurrentUserID);
		SetWindowText(str);		
	}	
}

void CMainFrame::OnOnhide() 
{
	if(GetApp()->m_bOnlineState==2)return;

	CData msg;
	msg.index=ONHIDE;
	msg.MyId=GetApp()->m_uCurrentUserID;
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;
	GetApp()->m_Socket.SendDataDirect(&msg);
}

void CMainFrame::OnOnline() 
{
	if(GetApp()->m_bOnlineState==1)return;
	CData msg;
	msg.index=ONLINE;
	msg.MyId=GetApp()->m_uCurrentUserID;
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;
	GetApp()->m_Socket.SendDataDirect(&msg);
}

void CMainFrame::OnSearch() 
{
	if(GetApp()->m_bOnlineState==0)
	{
		AfxMessageBox(IDS_NOT_LOGIN_SERVER);
		return;
	}

	if(m_pSearch==NULL)
	{
		m_pSearch=new CSearchDlg;
		m_pSearch->Create(CSearchDlg::IDD,GetDesktopWindow());
	}
	m_pSearch->ShowWindow(SW_NORMAL);	
}

void CMainFrame::OnPersoninfoSetup() 
{
	if(m_pModifyPI==NULL)
	{
		m_pModifyPI=new CModifyPIDlg;
		m_pModifyPI->Create(CModifyPIDlg::IDD,GetDesktopWindow());
	}
	m_pModifyPI->SendMessage(WM_SHOW_FRIEND_DETAIL,(WPARAM)(GetApp()->m_aUserInfo.GetAt(0)),0);		
}

void CMainFrame::OnMultisend() 
{
	if(GetApp()->m_bOnlineState==0)
	{
		AfxMessageBox(IDS_NOT_LOGIN_SERVER);
		return;
	}

	if(m_pMultiSend==NULL)
	{
		m_pMultiSend=new CMultiSendDlg;
		m_pMultiSend->Create(CMultiSendDlg::IDD,GetDesktopWindow());
	}
	m_pMultiSend->ShowWindow(SW_NORMAL);		
}

void CMainFrame::OnSendtoAll() 
{
	if(GetApp()->m_bOnlineState==0)
	{
		AfxMessageBox(IDS_NOT_LOGIN_SERVER);
		return;
	}
	if(m_pSendToAll==NULL)
	{
		m_pSendToAll=new CSendToAllDlg;
		m_pSendToAll->Create(CSendToAllDlg::IDD,GetDesktopWindow());
	}
	m_pSendToAll->ShowWindow(SW_NORMAL);	
}

void CMainFrame::OnMessageManage() 
{
	AfxMessageBox(IDS_NOT_FINISH);	
}

void CMainFrame::OnShowOnline() 
{
	if(GetApp()->m_bOnlineState==0)
	{
		AfxMessageBox(IDS_NOT_LOGIN_SERVER);
		return;
	}

	if(m_pShowOnline==NULL)
	{
		m_pShowOnline=new CShowOnlineDlg;
		m_pShowOnline->Create(CShowOnlineDlg::IDD,GetDesktopWindow());
	}
	m_pShowOnline->ShowWindow(SW_NORMAL);	
}

void CMainFrame::OnSendBroadcast() 
{
	if(GetApp()->m_bOnlineState==0)
	{
		CString str;
		str.LoadString(IDS_NOT_LOGIN_SERVER);
		MessageBox(str);
		return;
	}
	if(m_pSendBroad==NULL)
	{
		m_pSendBroad=new CSendBroadcastDlg;
		m_pSendBroad->Create(CSendBroadcastDlg::IDD,GetDesktopWindow());
	}
	m_pSendBroad->ShowWindow(SW_NORMAL);	
}

LRESULT CMainFrame::OnAddFriendRefresh(WPARAM wParam,LPARAM lParam)
{
	DWORD uFriendID=wParam;
	if(uFriendID<=0)return 0;
	if(FindInArray(GetApp()->m_aUserInfo,uFriendID)!=-1)return 0;

	UserInfo* pInfo=NULL;
	FriendState* pState=NULL;
	int rindex=FindInArray(GetApp()->m_aStranger,uFriendID);
	if(rindex!=-1)
	{
		pInfo=GetApp()->m_aStranger.GetAt(rindex);		
		pState=GetApp()->m_aStrangerState.GetAt(rindex);
		GetApp()->m_aStranger.RemoveAt(rindex);
		GetApp()->m_aStrangerState.RemoveAt(rindex);
		m_wndBar.RemoveAItem(1,uFriendID,FALSE);
	}
	else
	{
		pInfo=new UserInfo;				
		pInfo->Id=uFriendID;	
		pInfo->HaveDetail=0;
		pInfo->PhotoId=0;
		itoa(uFriendID,pInfo->Name,10);	
		pState=new FriendState;
		pState->OnlineState=0;
	}
	GetApp()->m_aUserInfo.Add(pInfo);					
	GetApp()->m_aUserState.Add(pState);	
	m_wndBar.AddItem(0,pInfo->Name,pInfo->PhotoId,pInfo->Id);	
	m_wndBar.Invalidate();
	if(!pInfo->HaveDetail)
	{
		CMsg1 msg;
		msg.index=FRIEND_DETAIL;
		msg.MyId=GetApp()->m_uCurrentUserID;
		msg.tarIP=GetApp()->m_uServerIP;
		msg.nPort=GetApp()->m_uServerPort;
		msg.FriendId=uFriendID;
		GetApp()->m_Socket.SendDataDirect(&msg);
	}
	return 1;
}

void CMainFrame::OnAddFriend() 
{
 	if(m_pAddFriendDlg==NULL)
	{
		m_pAddFriendDlg=new CAddFriendDlg;
		m_pAddFriendDlg->Create(CAddFriendDlg::IDD);
	}
	m_pAddFriendDlg->uID=nCurSelID;
	if(m_pAddFriendDlg->SendMessage(WM_NOTICE_ADDFRIEND))
	m_wndBar.RemoveAItem(1,nCurSelIndex);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam>'0' &&pMsg->wParam<'5'&&GetKeyState(VK_CONTROL)&0x80)
		{
			nCurSelIndex=pMsg->wParam-'1';
			nGroupFlag=m_wndBar.iSelFolder;				
			nCurSelID = m_wndBar.GetItemData(nCurSelIndex);
			if(nCurSelID!=0)SendMessage(WM_COMMAND,ID_SEND_MESSAGE,0);		
			return 1;
		}		
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}
