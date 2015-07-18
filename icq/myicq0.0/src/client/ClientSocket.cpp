// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket()
{
	strcpy(m_szResponseMsg,ResponseMsg);
	m_szrLength=strlen(m_szResponseMsg);	
	memset(m_arBuf,0,sizeof(m_arBuf));
	memset(m_apRecvWnd,0,sizeof(m_apRecvWnd));
	m_bBusy=FALSE;
	m_nTotalSend=0;	
	m_nCurRecvWnd=1;
}

CClientSocket::~CClientSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions
BOOL CClientSocket::Create(int TimeOut)
{	
	m_nTimeOut=TimeOut;
	return CAsyncSocket::Create(0,SOCK_DGRAM,FD_READ);
}

BOOL CClientSocket::SendData(CData *pData,BOOL &bCancel,CWnd* pRecvWnd,int FailReDoTimes)
{
	ASSERT(pData!=NULL);
	ASSERT(FailReDoTimes>=1);
	
	if(pRecvWnd!=NULL)
	{
		for(int i=1;i<m_nCurRecvWnd;i++)
		{
			if(m_apRecvWnd[i]==pRecvWnd)break;
		}
		if(i==m_nCurRecvWnd)
		{
			m_apRecvWnd[i]=pRecvWnd;
			pData->wIndex=i;
			m_nCurRecvWnd++;
			if(m_nCurRecvWnd==RecvWndNumber)m_nCurRecvWnd=1;
		}
		else
		{
			pData->wIndex=i;
		}
	}
	else
	{
		pData->wIndex=0;
	}

	int times=0,iCurIndex=0;
	BOOL bSendSuccess=FALSE;	
	
	m_nTotalSend++;
	
	iCurIndex=m_nTotalSend%CheckBufLength;
	m_arBuf[iCurIndex]=0;
	pData->This=iCurIndex;
	pData->PackToBuf();
	
	CString strIP;
	in_addr tIP;
	tIP.S_un.S_addr=pData->tarIP;
	strIP=inet_ntoa(tIP);

	while(times<FailReDoTimes&&!bSendSuccess&&!bCancel)
	{
		if(!m_bBusy)
		{
			m_bBusy=TRUE;						
			
			DWORD tBegin=GetTickCount();			
			SendTo(pData->szBuf,pData->num,pData->nPort,strIP);			

			m_bBusy=FALSE;
			MSG msg;
			while(!m_arBuf[iCurIndex]&&GetTickCount()-tBegin<m_nTimeOut&&!bCancel)
			{
				if(::PeekMessage(&msg,NULL,0,0,PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			bSendSuccess=m_arBuf[iCurIndex];	
			TRACE3("Send Data index:%d This:%d iCurI:%d\n",times,pData->This,iCurIndex);
			times++;			
		}
	}

	return bSendSuccess;	
}


void CClientSocket::OnReceive(int nErrorCode) 
{
	char buff[DataBufLength];
	int nRead;
	CString sIP;
	UINT nPort;
	
	nRead = ReceiveFrom(buff,DataBufLength,sIP,nPort);
	
	if (nRead != SOCKET_ERROR && nRead != 0 )
	{	
		if(StrNSame(buff,m_szResponseMsg,nRead-4,m_szrLength))
		{				
			DWORD Index=*((DWORD*)(buff+nRead-4));				
			if(Index<=CheckBufLength)
			{		
				m_arBuf[Index]=1;				
				return;
			}
		}		
		
		CData* pData=new CData;
		
		StrNCopy(pData->szBuf,buff,nRead);			
		pData->num=nRead;
		pData->nPort=nPort;
		pData->tarIP=inet_addr(sIP);
		
		if(pData->LoadFromBuf())
		{			
			StrNCopy(m_szResponseMsg+m_szrLength,(char*)&pData->This,sizeof(DWORD));
			SendTo(m_szResponseMsg,m_szrLength+sizeof(DWORD),nPort,sIP);
			if(pData->wIndex>=0&&pData->wIndex<RecvWndNumber&&m_apRecvWnd[pData->wIndex]!=NULL)
			{
				m_apRecvWnd[pData->wIndex]->PostMessage(WM_RECIEVE_MSG,(WPARAM)pData,0);
			}
			TRACE2("Recv a Data index: %d This: %d\n",pData->index,pData->This);
		}
	}	
}

void CClientSocket::SendDataDirect(CData *pData)
{
	ASSERT(pData!=NULL);
	m_nTotalSend++;		
	pData->This=m_nTotalSend%CheckBufLength;		
	pData->PackToBuf();
	CString strIP;
	in_addr tIP;
	tIP.S_un.S_addr=pData->tarIP;
	strIP=inet_ntoa(tIP);
	
	SendTo(pData->szBuf,pData->num,pData->nPort,strIP);			
}
