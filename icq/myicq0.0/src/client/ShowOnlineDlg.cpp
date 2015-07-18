// ShowOnlineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "ShowOnlineDlg.h"
#include "FriendDetail.h"
#include "addfrienddlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowOnlineDlg dialog


CShowOnlineDlg::CShowOnlineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowOnlineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowOnlineDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	hIcon=AfxGetApp()->LoadIcon(IDR_SEARCH);
	m_pDlg=NULL;
	bStartSearch=FALSE;
}


void CShowOnlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowOnlineDlg)
	DDX_Control(pDX, IDC_NEXTPAGE, m_nextbutton);
	DDX_Control(pDX, IDC_LASTPAGE, m_lastbutton);
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowOnlineDlg, CDialog)
	//{{AFX_MSG_MAP(CShowOnlineDlg)
	ON_BN_CLICKED(IDC_NEXTPAGE, OnNextpage)
	ON_BN_CLICKED(IDC_LASTPAGE, OnLastpage)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_COMMAND(ID_ADD_FRIEND, OnAddFriend)
	ON_COMMAND(ID_FRIENDS_DETAIL, OnFriendDetail)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECIEVE_MSG,OnRecvShowOnline)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowOnlineDlg message handlers

BOOL CShowOnlineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);
	CMenu* pMenu=GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_RESTORE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED|MF_DISABLED);
	pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED|MF_DISABLED);
	
	CRect			rect;
	LV_COLUMN		lvcolumn;
	TCHAR			rgtsz[3][10] = {_T("∫≈¬Î"), _T("–’√˚"), _T("≤ø√≈")};

	m_List.GetClientRect(&rect);
	
	int cx[3]={rect.Width()*3/10,rect.Width()*3/10,rect.Width()*4/10};
	for (int i=0;i<3;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;	
		lvcolumn.cx=cx[i];
		m_List.InsertColumn(i, &lvcolumn);
	}	
	m_List.SetImageList(&GetApp()->m_imaSmallHead,LVSIL_SMALL);
	msg.index=APPLY_SHOW_ONLINE;
	msg.MyId=GetApp()->m_uCurrentUserID;
	msg.tarIP=GetApp()->m_uServerIP;
	msg.nPort=GetApp()->m_uServerPort;
	msg.Value=0;
	m_lastbutton.EnableWindow(FALSE);
	bClose=TRUE;
	return TRUE;
}

void CShowOnlineDlg::OnCancel() 
{
 	bCancel=TRUE;
	bClose=TRUE;
	msg.Value=0;
	m_lastbutton.EnableWindow(FALSE);
	m_nextbutton.EnableWindow();
	m_List.DeleteAllItems();
	CDialog::OnCancel();
}

void CShowOnlineDlg::OnClose() 
{
	OnCancel();
}

void CShowOnlineDlg::OnOK() 
{
	OnCancel();
}

void CShowOnlineDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(!bShow)return;

	if(bClose)
	{
		msg.Value=0;
		OnRefresh();
	}
	bClose=FALSE;	
}

void CShowOnlineDlg::OnRefresh() 
{	
	if(!GetApp()->m_bOnlineState)
	{
		CString str;
		str.LoadString(IDS_NOT_LOGIN_SERVER);
		MessageBox(str);
		return;
	}
	if(int(msg.Value*PersonNumEveryTime)<=GetApp()->m_nTotalOnline)
	{
		m_List.DeleteAllItems();
		bCancel=FALSE;
		if(GetApp()->m_Socket.SendData(&msg,bCancel,this))
		{
			bStartSearch=TRUE;
			BeginWaitCursor();
			SetTimer(1,TimeWaitForSearch,NULL);
		}
		else
		{
			CString str;
			str.LoadString(IDS_SERVER_NOT_RES);
			MessageBox(str);
			return;
		}
	}		
}

void CShowOnlineDlg::OnNextpage() 
{
	if(int((msg.Value+1)*PersonNumEveryTime)<=GetApp()->m_nTotalOnline)
	{
		msg.Value++;
		m_lastbutton.EnableWindow();		
		OnRefresh();
	}
}

void CShowOnlineDlg::OnLastpage() 
{
	if(msg.Value>0)
	{
		msg.Value--;
		if(msg.Value>0)m_lastbutton.EnableWindow();
		else m_lastbutton.EnableWindow(FALSE);
		OnRefresh();
	}	
}

void CShowOnlineDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
	{
		bStartSearch=FALSE;
		KillTimer(1);
		EndWaitCursor();
	} 
}

LRESULT CShowOnlineDlg::OnRecvShowOnline(WPARAM wParam,LPARAM lParam)
{
	CData *pData=(CData*)wParam;

	if(pData->index==RECV_SHOW_ONLINE&&bStartSearch)
	{
		bStartSearch=FALSE;
		EndWaitCursor();
		CShowOnlinePeople msg1;
		CopyDataBuf(&msg1,pData);
		if(!msg1.LoadFromBuf())return 0;		
	
		LVITEM lv;
		int iPos;
		char temp[100];
		for(int k=0;k<msg1.aId.GetSize();k++)
		{				
			lv.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
			lv.iImage=msg1.aPhotoId.GetAt(k);
			lv.iSubItem=0;
			lv.iItem=k;
			itoa(msg1.aId.GetAt(k),temp,10);
			lv.pszText=temp;				
			lv.lParam=msg1.aId.GetAt(k);
			iPos=m_List.InsertItem(&lv);
			lv.mask=LVIF_TEXT;
			lv.iItem=iPos;
			lv.iSubItem=1;
			lstrcpy(temp,msg1.aName.GetAt(k));
			lv.pszText=temp;
			m_List.SetItem(&lv);
			lv.iSubItem=2;
			lstrcpy(temp,msg1.aDepartment.GetAt(k));
			lv.pszText=temp;
			m_List.SetItem(&lv);
		}
		TRACE0("IS RECV SHOW ONLINE");
		return 1;
	}
	return 0;
}

void CShowOnlineDlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos=m_List.GetFirstSelectedItemPosition();
	if(pos==NULL)return;

	CMenu menu,*pMenu=NULL;	

	if(!menu.LoadMenu(IDR_POPMENU))
		return;	
	pMenu=menu.GetSubMenu(1);
		
	CPoint mouse;
	GetCursorPos(&mouse);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,			
			mouse.x,
			mouse.y,
			this);		
	*pResult = 0;
}

void CShowOnlineDlg::OnAddFriend() 
{
	POSITION pos=m_List.GetFirstSelectedItemPosition();
	if(pos==NULL)return;
	int index=m_List.GetNextSelectedItem(pos);	
	
	CAddFriendDlg *& pDlg=((CMainFrame*)GetApp()->m_pMainWnd)->m_pAddFriendDlg;
	if(pDlg==NULL)
	{
		pDlg=new CAddFriendDlg;
		pDlg->Create(CAddFriendDlg::IDD);
	}
	pDlg->uID=m_List.GetItemData(index);	
	pDlg->SendMessage(WM_NOTICE_ADDFRIEND);	
}

void CShowOnlineDlg::OnFriendDetail() 
{
	memset(&Info,0,sizeof(Info));
	POSITION pos=m_List.GetFirstSelectedItemPosition();
	if(pos==NULL)return;
	int index=m_List.GetNextSelectedItem(pos);
	Info.Id=m_List.GetItemData(index);
	Info.HaveDetail=0;
	if(m_pDlg==NULL)
	{
		m_pDlg=new CFriendDetailDlg;
		m_pDlg->Create(CFriendDetailDlg::IDD,GetDesktopWindow());		
	}	
	m_pDlg->SendMessage(WM_SHOW_FRIEND_DETAIL,(WPARAM)&Info,0);		
	m_pDlg->OnRefresh();	
}

void CShowOnlineDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos=m_List.GetFirstSelectedItemPosition();
	if(pos!=NULL)PostMessage(WM_COMMAND,ID_FRIENDS_DETAIL,0);	
	*pResult = 0;
}
