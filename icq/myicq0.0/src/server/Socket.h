#if !defined(AFX_SOCKET_H__CC9C0986_49F0_11D5_B110_97ACDB42FA00__INCLUDED_)
#define AFX_SOCKET_H__CC9C0986_49F0_11D5_B110_97ACDB42FA00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Socket.h : header file
//
#include "common.h"

class CData;
/////////////////////////////////////////////////////////////////////////////
// CRecvSocket command target

class CRecvSocket : public CAsyncSocket
{
// Attributes
private:
	char m_szResponseMsg[MaxResponseMsgLength];
	int m_szrLength;

// Operations
public:
	CRecvSocket();
	virtual ~CRecvSocket();

// Overrides
public:
	BOOL Create(int nPort);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecvSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CRecvSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CSendSocket command target

class CSendSocket : public CAsyncSocket
{
// Attributes
private:
	BYTE* pBuf;
	char m_szResponseMsg[20];
	int m_szrLength;
// Operations
public:
	CSendSocket(BYTE*buf);
	virtual ~CSendSocket();
	BOOL Create();
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSendSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:

};

/////////////////////////////////////////////////////////////////////////////

class CServerSocket  
{
public:	
	void CloseSendSocket();
	void CloseListenSocket();
	BOOL Create(int SendSockNum,CArray<DWORD,DWORD>&aPort,int TimeOut=TimeWaitForRes);
	BOOL SendData(CData* pData,int FailReDoTimes=3);
	CServerSocket();
	virtual ~CServerSocket();
private:
	CRecvSocket* m_apRecvSocket[ListenSocketNum];
	CSendSocket* m_apSendSocket[SendSocketNum];
	int m_nRecvSocket;
	int m_nSendSocket;
	BOOL m_abBusy[SendSocketNum];
	BYTE m_arBuf[CheckBufLength];	
	DWORD m_nTotalSend;
	DWORD m_nTimeOut;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKET_H__CC9C0986_49F0_11D5_B110_97ACDB42FA00__INCLUDED_)
