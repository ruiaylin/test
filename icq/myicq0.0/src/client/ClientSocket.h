#if !defined(AFX_CLIENTSOCKET_H__C23B8BB3_5051_11D5_B131_C6D7FE5A2900__INCLUDED_)
#define AFX_CLIENTSOCKET_H__C23B8BB3_5051_11D5_B131_C6D7FE5A2900__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CClientSocket command target

class CClientSocket : public CAsyncSocket
{
// Attributes
public:


// Operations
public:
	CClientSocket();
	virtual ~CClientSocket();

// Overrides
public:
	void SendDataDirect(CData* pData);	
	BOOL Create(int TimeOut=TimeWaitForRes);
	BOOL SendData(CData* pData,BOOL &bCancel,CWnd* pRecvWnd=NULL,int FailReDoTimes=3);
	CWnd* GetOwner(){return m_apRecvWnd[0];};
	void SetOwner(CWnd* pWnd){m_apRecvWnd[0]=pWnd;};
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
private:	
	char m_szResponseMsg[MaxResponseMsgLength];
	int m_szrLength;
	BOOL m_bBusy;
	BYTE m_arBuf[CheckBufLength];
	DWORD m_nTotalSend;	
	DWORD m_nTimeOut;
	CWnd * m_apRecvWnd[RecvWndNumber];
	int m_nCurRecvWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__C23B8BB3_5051_11D5_B131_C6D7FE5A2900__INCLUDED_)
