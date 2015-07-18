////////////////////////////////////////////////////////////////
//															  //	
//		给用户发在线人数的函数，同时检测用户是否仍在线		  //	
//															  //
//															  //	
////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "common.h"
#include "msg.h"
#include "Server.h"
#include "MainFrm.h"
#include "ServerView.h"


UINT CheckOnline(LPVOID param)
{
	CServerApp * pApp=(CServerApp*)AfxGetApp();				
	UserOnline * pUsers=pApp->m_pUsers;
	CDatabase * pDatabase=&pApp->m_Database;
	CServerSocket * pSocket=&pApp->m_Socket;	
	
	while(pApp->m_bServerStarted)
	{
		Sleep(TimerSpanServer);		
		for(int i=0;i<pApp->m_nTotalUserNumber&&pApp->m_bServerStarted;i++)
		{
			if(!pUsers[i].State)continue;
			
			CMsg3 msg;
			msg.index=TOTAL_ONLINE;
			msg.tarIP=pUsers[i].IP;
			msg.nPort=pUsers[i].Port;
			msg.Value=pApp->m_nNumberOnline;
			BOOL bSu=pSocket->SendData(&msg);
			if(bSu)continue;			
			
			pUsers[i].State=0;			
			::InterlockedDecrement((LPLONG)&pApp->m_nNumberOnline);			
			
			CRecordset recordset(pDatabase);
			CString mysql;
			mysql.Format("select MyId from Friends where FriendId=%d",pUsers[i].Id);
			try
			{
				recordset.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
			}
			catch(CDBException e)
			{
				AfxMessageBox(e.m_strError);
				break;
			}
			CDBVariant value;
			CArray<DWORD,DWORD>pFriendId;
			while(!recordset.IsEOF())
			{
				recordset.GetFieldValue(short(0),value);
				pFriendId.Add(value.m_lVal);
				recordset.MoveNext();
			}
			recordset.Close();				
			
			// 给在线的好友发OFFLINE消息
			CData msg1;
			msg1.index=OFFLINE;
			msg1.MyId=pUsers[i].Id;
			for(int j=0;j<pFriendId.GetSize()&&pApp->m_bServerStarted;j++)
			{
				int pi=pFriendId.GetAt(j)-UserIdRadix;						
				if(pUsers[pi-1].State&&pApp->m_bServerStarted)
				{
					msg.tarIP=pUsers[pi-1].IP;
					msg.nPort=pUsers[pi-1].Port;
					pSocket->SendData(&msg1);					
				}
			}
		}
	}
	
	return 0;
}
