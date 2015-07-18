////////////////////////////////////////////////////////////////
//		FileName: Socket.cpp								  //	
//															  //	
//		服务器的底层通讯CServerSocket类						  //
//															  //	
////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "common.h"
#include "Socket.h"
#include "data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecvSocket

CRecvSocket::CRecvSocket()
{
	strcpy(m_szResponseMsg,ResponseMsg);
	m_szrLength=strlen(m_szResponseMsg);
}

CRecvSocket::~CRecvSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CRecvSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CRecvSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CRecvSocket member functions
BOOL CRecvSocket::Create(int nPort)
{
	TRACE1("Create Listen Socket : %d\n",nPort);
	return CAsyncSocket::Create(nPort,SOCK_DGRAM,FD_READ|FD_CLOSE);
}


void CRecvSocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	OutputDebugString("CUdpSocket::OnReceive\n");
	char buff[DataBufLength];
	int nRead;
	CString sIP;
	UINT nPort;

	nRead = ReceiveFrom(buff,DataBufLength, sIP, nPort);
	
	if (nRead != SOCKET_ERROR && nRead != 0 )
	{	
		CData* pData=new CData;
		
		StrNCopy(pData->szBuf,buff,nRead);			
		pData->num=nRead;
		pData->nPort=nPort;
		pData->tarIP=inet_addr(sIP);

		if(pData->LoadFromBuf())
		{			
			StrNCopy(m_szResponseMsg+m_szrLength,(char*)&pData->This,sizeof(DWORD));
			SendTo(m_szResponseMsg,m_szrLength+sizeof(DWORD),nPort,sIP);
			AfxBeginThread(ProcessRecvData,(LPVOID)pData);
		}
	}	

	CAsyncSocket::OnReceive(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
// CSendSocket

CSendSocket::CSendSocket(BYTE*buf):pBuf(buf)
{
	strcpy(m_szResponseMsg,ResponseMsg);
	m_szrLength=strlen(m_szResponseMsg);
}

CSendSocket::~CSendSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CSendSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CSendSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CSendSocket member functions
BOOL CSendSocket::Create()
{
	return CAsyncSocket::Create(0,SOCK_DGRAM,FD_READ);
}


void CSendSocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	char buff[DataBufLength];
	int nRead;
	CString sIP;
	UINT nPort;

	nRead = ReceiveFrom(buff,DataBufLength, sIP, nPort);

	if (nRead != SOCKET_ERROR && nRead != 0 )
	{		
		// Check if is the response message
		if(StrNSame(buff,m_szResponseMsg,nRead-4,m_szrLength))
		{				
			DWORD Index=*((DWORD*)(buff+nRead-4));			
			if(Index<CheckBufLength)  // find it
			{		
				pBuf[Index]=1;
				TRACE1("Recv Res this: %d \n",Index);
			}			
		}	
	}	
	
	CAsyncSocket::OnReceive(nErrorCode);
}



//////////////////////////////////////////////////////////////////////
// CServerSocket Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerSocket::CServerSocket()
{
	m_nRecvSocket=0;
	m_nSendSocket=0;
	memset(m_abBusy,0,sizeof(m_abBusy));
	memset(m_arBuf,0,sizeof(m_arBuf));
	m_nTotalSend=0;	
}

CServerSocket::~CServerSocket()
{
}

BOOL CServerSocket::Create(int SendSockNum,CArray<DWORD,DWORD>&aRecvPort,int TimeOut)
{
	m_nSendSocket=SendSockNum;
	m_nRecvSocket=aRecvPort.GetSize();
	m_nTimeOut=TimeOut;

	ASSERT(m_nRecvSocket>=1&&m_nSendSocket>=1);
	
	BOOL CreateSuccess=TRUE;

	for(int i=0;i<m_nSendSocket&&CreateSuccess;i++)
	{
		m_apSendSocket[i]=new CSendSocket(m_arBuf);
		CreateSuccess=m_apSendSocket[i]->Create();
	}
	for(int j=0;j<m_nRecvSocket&&CreateSuccess;j++)
	{
		m_apRecvSocket[j]=new CRecvSocket();
		CreateSuccess=m_apRecvSocket[j]->Create(aRecvPort.GetAt(j));
	}	

	return CreateSuccess;

}
BOOL CServerSocket::SendData(CData *pData,int FailReDoTimes)
{
	ASSERT(pData!=NULL);
	ASSERT(m_nSendSocket&&m_nRecvSocket);	

	int times=0,iCurIndex;
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

	for(int i=0;i<m_nSendSocket&&times<FailReDoTimes&&!bSendSuccess;i++,i%=m_nSendSocket)
		if(!m_abBusy[i])
		{
			m_abBusy[i]=TRUE;								
			
			DWORD tBegin=GetTickCount();
			m_apSendSocket[i]->SendTo(pData->szBuf,pData->num,pData->nPort,strIP);
			
			m_abBusy[i]=FALSE;

			while(!m_arBuf[iCurIndex]&&GetTickCount()-tBegin<m_nTimeOut);			
			bSendSuccess=m_arBuf[iCurIndex];
			TRACE3("Send Data index:%d This:%d iCurI:%d\n",pData->index,pData->This,iCurIndex);
			times++;
		}	
	
	return bSendSuccess;	
}



void CRecvSocket::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	TRACE0("CRecvSocket :: Onclose \n");
	CAsyncSocket::OnClose(nErrorCode);
}

void CServerSocket::CloseListenSocket()
{
	for(int i=0;i<m_nRecvSocket;i++)
	{
		m_apRecvSocket[i]->Close();
		delete m_apRecvSocket[i];
	}
}

void CServerSocket::CloseSendSocket()
{
	for(int i=0;i<m_nSendSocket;i++)
	{
		m_apSendSocket[i]->Close();
		delete m_apSendSocket[i];
	}
}
