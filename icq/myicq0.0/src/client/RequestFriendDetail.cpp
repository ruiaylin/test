#include "stdafx.h"
#include "msg.h"
#include "common.h"
#include "Client.h"

UINT RequestFriendDetail(LPVOID param)
{	
	CClientApp* pApp=(CClientApp*)AfxGetApp();
	Sleep(4000);
	for(int i=0;i<pApp->m_aUserInfo.GetSize()&&pApp->m_bOnlineState;i++)
	{
		if(pApp->m_aUserInfo.GetAt(i)->HaveDetail==0)
		{
			CMsg1 msg;
			msg.index=FRIEND_DETAIL;
			msg.MyId=pApp->m_uCurrentUserID;
			msg.tarIP=pApp->m_uServerIP;
			msg.nPort=pApp->m_uServerPort;
			msg.FriendId=pApp->m_aUserInfo.GetAt(i)->Id;
			pApp->m_Socket.SendDataDirect(&msg);
			Sleep(2000);
		}
	}	
	return 1;
}