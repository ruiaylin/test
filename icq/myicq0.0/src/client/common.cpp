#include "stdafx.h"
#include "common.h"
#include "msg.h"
#include "FriendDetail.h"
#include "TalkDlg.h"
#include "LookDlg.h"

FriendState::FriendState()
{
	OnlineState=0;
	IP=Port=0;	
	pTalk=NULL;
	pRecv=NULL;
	pFInfo=NULL;	
	nCurrentMsg=0;
}

FriendState::~FriendState()
{
	for(int i=0;i<aMsg.GetSize();i++)
	{
		delete aMsg.GetAt(i);
	}
	aMsg.RemoveAll();
	if(pFInfo!=NULL)delete pFInfo;
	if(pRecv!=NULL)delete pRecv;
	if(pTalk!=NULL)delete pTalk;
}
UserInfo::UserInfo()
{
	memset(this,0,sizeof(UserInfo));
}
