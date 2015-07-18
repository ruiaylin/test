// ServerView.cpp : implementation of the CServerView class
//

#include "stdafx.h"
#include "Server.h"
#include "msg.h"

#include "ServerDoc.h"
#include "ServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerView

IMPLEMENT_DYNCREATE(CServerView, CEditView)

BEGIN_MESSAGE_MAP(CServerView, CEditView)
	//{{AFX_MSG_MAP(CServerView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerView construction/destruction

CServerView::CServerView()
{
	// TODO: add construction code here

}

CServerView::~CServerView()
{
}

BOOL CServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

/////////////////////////////////////////////////////////////////////////////
// CServerView drawing

void CServerView::OnDraw(CDC* pDC)
{
	CServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CServerView diagnostics

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CEditView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CServerDoc* CServerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerDoc)));
	return (CServerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CServerView message handlers

void CServerView::ShowMessage(CString str)
{	
	CEdit& Edit=GetEditCtrl();
	m_criticalShowMess.Lock();
	int len=Edit.GetWindowTextLength();
	Edit.SetSel(len,len);		
	Edit.ReplaceSel(str+"\r\n");
	m_criticalShowMess.Unlock();
}

void CServerView::OnEditCopy() 
{
	CServerApp * pApp=(CServerApp*)AfxGetApp();				
	
	UserOnline * pUsers=pApp->m_pUsers;	
	CServerSocket * pSocket=&pApp->m_Socket;
	int &NumberOnline=pApp->m_nNumberOnline;
	int &MaxUserId=pApp->m_nMaxUserId;
	for(int i=0;i<MaxUserId;i++)
	{
		if(pUsers[i].State)
		{			
			CMsg3 msg;
			msg.index=TOTAL_ONLINE;
			msg.tarIP=pUsers[i].IP;
			msg.nPort=pUsers[i].Port;
			msg.Value=100;
			CString str;
			if(!pSocket->SendData(&msg))
			{
				str.Format("Send Error :%d",i);
				MessageBox(str);
			}
			else
			{
				str.Format("Send Ok : %d",i);
				MessageBox(str);
			}
		}
	}	
}
