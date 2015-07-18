// SearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "SearchDlg.h"
#include "frienddetail.h"
#include "addfrienddlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog


CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchDlg)
	m_RID = -1;
	//}}AFX_DATA_INIT
	hIcon=AfxGetApp()->LoadIcon(IDR_SEARCH);
	m_pDlg=NULL;
	bStartSearch=FALSE;
}


void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchDlg)
	DDX_Control(pDX, IDC_BYNAME, m_Name);
	DDX_Control(pDX, IDC_BYID, m_ID);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Radio(pDX, IDC_RADIO_ID, m_RID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_BN_CLICKED(IDC_RADIO_ID, OnRadioId)
	ON_BN_CLICKED(IDC_RADIO_NAME, OnRadioName)
	ON_COMMAND(ID_FRIENDS_DETAIL, OnFriendsDetail)
	ON_COMMAND(ID_ADD_FRIEND, OnAddFriend)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP	
	ON_MESSAGE(WM_RECIEVE_MSG,OnRecvSearch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg message handlers

BOOL CSearchDlg::OnInitDialog() 
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
	
	m_RID=0;
	m_Name.EnableWindow(FALSE);
	m_List.SetImageList(&GetApp()->m_imaSmallHead,LVSIL_SMALL);
	UpdateData(FALSE);

	return TRUE;  
}

void CSearchDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	PostMessage(WM_COMMAND,ID_FRIENDS_DETAIL,0);
	*pResult = 0;
}

void CSearchDlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
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

void CSearchDlg::OnRadioId() 
{	
	m_Name.EnableWindow(FALSE);
	m_ID.EnableWindow(TRUE);
	m_Name.SetWindowText("");
}

void CSearchDlg::OnRadioName() 
{
	m_Name.EnableWindow(TRUE);
	m_ID.EnableWindow(FALSE);	
	m_ID.SetWindowText("");
}

void CSearchDlg::OnFriendsDetail() 
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

void CSearchDlg::OnAddFriend() 
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

LRESULT CSearchDlg::OnRecvSearch(WPARAM wParam,LPARAM lParam)
{
	CData *pData=(CData*)wParam;
	switch(pData->index)
	{
	case FOUND_FRIEND_BY_ID:
		{			
			if(!bStartSearch)break;
			bStartSearch=FALSE;
			CMsgPerson msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
	
			m_List.DeleteAllItems();
			LVITEM lv;
			int iPos;
			char temp[100];
			lv.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
			lv.iImage=msg.PhotoId;
			lv.iSubItem=0;
			lv.iItem=0;
			itoa(msg.MyId,temp,10);
			lv.pszText=temp;
			lv.lParam=msg.MyId;
			iPos=m_List.InsertItem(&lv);
			lv.mask=LVIF_TEXT;
			lv.iItem=iPos;
			lv.iSubItem=1;
			lstrcpy(temp,msg.Name);
			lv.pszText=temp;
			m_List.SetItem(&lv);
			lv.iSubItem=2;
			lstrcpy(temp,msg.Department);
			lv.pszText=temp;
			m_List.SetItem(&lv);			
		}
	case FOUND_FRIEND_BY_NAME:
		{
			if(!bStartSearch)break;
			bStartSearch=FALSE;
			CShowOnlinePeople msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			m_List.DeleteAllItems();
			LVITEM lv;
			int iPos;
			char temp[100];
			for(int k=0;k<msg.aId.GetSize();k++)
			{				
				lv.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
				lv.iImage=msg.aPhotoId.GetAt(k);
				lv.iSubItem=0;
				lv.iItem=k;
				itoa(msg.aId.GetAt(k),temp,10);
				lv.pszText=temp;				
				lv.lParam=msg.aId.GetAt(k);
				iPos=m_List.InsertItem(&lv);
				lv.mask=LVIF_TEXT;
				lv.iItem=iPos;
				lv.iSubItem=1;
				lstrcpy(temp,msg.aName.GetAt(k));
				lv.pszText=temp;
				m_List.SetItem(&lv);
				lv.iSubItem=2;
				lstrcpy(temp,msg.aDepartment.GetAt(k));
				lv.pszText=temp;
				m_List.SetItem(&lv);
			}			
		}
	}

	if(pData!=NULL)
		delete pData;
	
	return 1;
}

void CSearchDlg::OnOK() 
{
	UpdateData();
	
	CString strID,strName;
	DWORD uID=0;
	
	if(m_RID==0)
	{
		if(m_ID.GetWindowTextLength()==0)
		{
			m_ID.SetFocus();
			return;
		}
		m_ID.GetWindowText(strID);
		
		uID=atoi(strID);
		if(uID>0)
		{
			CMsg1 msg;
			msg.index=FIND_FRIEND_BY_ID;
			msg.MyId=GetApp()->m_uCurrentUserID;
			msg.FriendId=uID;
			msg.tarIP=GetApp()->m_uServerIP;
			msg.nPort=GetApp()->m_uServerPort;
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
		else
		{
			MessageBox("IDÃÓ–¥¥ÌŒÛ!");
			m_ID.SetWindowText("");
			m_ID.SetFocus();
			return;
		}
	}
	else
	{
		m_Name.GetWindowText(strName);
		if(strName.IsEmpty())
		{
			m_Name.SetFocus();
			return;
		}
		CMsg4 msg;
		msg.index=FIND_FRIEND_BY_NAME;		
		msg.MyId=GetApp()->m_uCurrentUserID;
		msg.Msg=strName;		
		msg.tarIP=GetApp()->m_uServerIP;
		msg.nPort=GetApp()->m_uServerPort;
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

void CSearchDlg::OnClose() 
{
	OnCancel();
}

void CSearchDlg::OnCancel() 
{
	bCancel=TRUE;
	m_List.DeleteAllItems();	
	m_ID.EnableWindow();
	m_Name.EnableWindow(FALSE);
	m_ID.SetWindowText("");
	m_Name.SetWindowText("");
	((CButton*)GetDlgItem(IDC_RADIO_ID))->SetCheck(1);
	CDialog::OnCancel();
}
