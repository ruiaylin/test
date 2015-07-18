////////////////////////////////////////////////////////////////
//															  //	
//		处理客户请求的工人线程函数							  //	
//															  //
//															  //	
////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "common.h"
#include "msg.h"
#include "Server.h"
#include "MainFrm.h"
#include "ServerView.h"


UINT ProcessRecvData(LPVOID param)
{
	CData* pData=(CData*)param;
	UINT index=pData->index;
	
	CServerApp * pApp=(CServerApp*)AfxGetApp();	
	CMainFrame* pMainWnd=(CMainFrame*)(pApp->m_pMainWnd);
	CServerView *pView=(CServerView*)(pMainWnd->GetActiveView());

	UserOnline * pUsers=pApp->m_pUsers;
	CDatabase * pDatabase=&pApp->m_Database;
	CServerSocket * pSocket=&pApp->m_Socket;

	::InterlockedIncrement((long*)&pApp->m_nRecvMsg);

	switch(index)
	{
	case ONHIDE:
		{
			int pIndex=pData->MyId-UserIdRadix;
			if(pIndex>=0&&pIndex<pApp->m_nTotalUserNumber)  // true means it esxit
			{
				if(pUsers[pIndex].State==0||pUsers[pIndex].State==1)
				{
					CData mc;
					mc.index=ONHIDE_OK;
					mc.MyId=pData->MyId;
					mc.tarIP=pData->tarIP;
					mc.nPort=pData->nPort;
					mc.wIndex=pData->wIndex;
					if(!pSocket->SendData(&mc))break;					
					
					if(pUsers[pIndex].State==0)
					{
						//找出我的朋友
						CRecordset recordset(pDatabase);
						CString mysql;
						mysql.Format("select FriendId from Friends where MyId=%d",pUsers[pIndex].Id);
						recordset.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);						
						CDBVariant value;
						CArray<DWORD,DWORD>pFriendId;
						while(!recordset.IsEOF())
						{
							recordset.GetFieldValue(short(0),value);
							pFriendId.Add(value.m_lVal);
							recordset.MoveNext();
						}
						recordset.Close();
						
						// 发送好友列表给用户
						CMsg2 msg;
						msg.index=ALL_FRIEND_ID;
						msg.wIndex=pData->wIndex;
						msg.tarIP=pData->tarIP;
						msg.nPort=pData->nPort;
						msg.MyId=pData->MyId;
						msg.aFriendId.Append(pFriendId);
						if(!pSocket->SendData(&msg))break;												
						
						// 发送在线好友给用户
						CMsgOnlineFriend msg1;
						msg1.index=ONLINE_FRIEND;
						msg1.wIndex=pData->wIndex;
						msg1.tarIP=pData->tarIP;
						msg1.nPort=pData->nPort;
						msg1.MyId=pData->MyId;
						for(int i=0;i<pFriendId.GetSize();i++)
						{
							int pi=pFriendId.GetAt(i)-UserIdRadix;
							if(pUsers[pi].State)
							{
								msg1.aFriendId.Add(pUsers[pi].Id);
								msg1.aFriendIP.Add(pUsers[pi].IP);
								msg1.aFriendPort.Add(pUsers[pi].Port);
								msg1.aFriendState.Add(pUsers[pi].State);
							}
						}
						Sleep(500);
						if(!pSocket->SendData(&msg1))break;							
					
						//发送离线广播消息													
						mysql.Format("select broadcast.SendTime,broadcast.SenderId,broadcast.msg,offbroadcast.num from broadcast,offbroadcast where offbroadcast.msgid=broadcast.msgid and offbroadcast.RecvId=%d",pData->MyId);
						CRecordset rs(pDatabase);
						rs.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);						
						CMsg1 msg3;
						CDBVariant s1;
						msg3.tarIP=pData->tarIP;
						msg3.nPort=pData->nPort;
						msg3.index=SYSTEM_BROADCAST;
						msg3.wIndex=pData->wIndex;
						msg3.FriendId=pData->MyId;
						
						while(!rs.IsEOF())
						{
							rs.GetFieldValue(short(0),s1);
							msg3.Time=s1.m_lVal;
							rs.GetFieldValue(1,s1);
							msg3.MyId=s1.m_lVal;
							rs.GetFieldValue(2,msg3.Msg);
							rs.GetFieldValue(3,s1);								
							
							int inum=0;
							inum=s1.m_lVal;
							Sleep(500);
							if(pSocket->SendData(&msg3))
							{
								CString sql1;
								sql1.Format("delete from offbroadcast where num=%d",inum);
								pDatabase->ExecuteSQL(sql1);
							}
							else
							{
								rs.Close();
								goto proc_end;
							}
							rs.MoveNext();
						}
						rs.Close();					

						//发送离线消息							
						mysql.Format("select * from offmsg where RecvId=%d",pData->MyId);						
						rs.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
						
						CMsg1 offmsg3;
						CDBVariant dbMsgId,dbSenderId,dbRecvId,dbRecvTime,dbmIndex;						
						offmsg3.tarIP=pData->tarIP;
						offmsg3.nPort=pData->nPort;							
						offmsg3.FriendId=pData->MyId;
						offmsg3.wIndex=pData->wIndex;
						
						while(!rs.IsEOF())
						{
							rs.GetFieldValue("MsgId",dbMsgId);
							rs.GetFieldValue("SenderId",dbSenderId);
							rs.GetFieldValue("RecvId",dbRecvId);
							rs.GetFieldValue("RecvTime",dbRecvTime);
							rs.GetFieldValue("mIndex",dbmIndex);
							rs.GetFieldValue("Msg",offmsg3.Msg);
							offmsg3.MyId=dbSenderId.m_lVal;
							offmsg3.Time=dbRecvTime.m_lVal;
							offmsg3.index=dbmIndex.m_chVal;								
							
							int inum=0;
							inum=dbMsgId.m_lVal;
							Sleep(500);
							if(pSocket->SendData(&offmsg3))
							{
								CString sql1;
								sql1.Format("delete from offmsg where MsgID=%d",inum);
								pDatabase->ExecuteSQL(sql1);
							}
							else
							{
								rs.Close();
								goto proc_end;
							}
							rs.MoveNext();
						}
						rs.Close();						
						
						pUsers[pIndex].IP=pData->tarIP;
						pUsers[pIndex].Port=pData->nPort;
						::InterlockedIncrement((LPLONG)&pApp->m_nNumberOnline);
					}

					pUsers[pIndex].State=2;

					//找出把我当为好友的人，给其中在线或隐身的人发送我的隐身消息
					CRecordset recordset(pDatabase);
					CString mysql;
					mysql.Format("select MyId from Friends where FriendId=%d",pUsers[pIndex].Id);
					recordset.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);					
					CDBVariant value;
					CNoticeMsg msg;
					msg.index=SOMEONE_ONHIDE;
					msg.MyId=pData->MyId;
					msg.IP=pData->tarIP;
					msg.Port=pData->nPort;
					while(!recordset.IsEOF())
					{
						recordset.GetFieldValue(short(0),value);
						int pi=value.m_lVal-UserIdRadix;
						if(pUsers[pi].State)
						{
							msg.tarIP=pUsers[pi].IP;
							msg.nPort=pUsers[pi].Port;
							pSocket->SendData(&msg);					
						}
						recordset.MoveNext();
					}
					recordset.Close();					
				}
				CString str;
				in_addr tIP;
				tIP.S_un.S_addr=pData->tarIP;
				str.Format("ID: %d 隐身登陆,IP: %s,Port: %d",pData->MyId,inet_ntoa(tIP),pData->nPort);				
#ifdef _DEBUG
				pView->ShowMessage(str);
#endif
			}
			else
			{
				CString str;
				str.Format("ID: %d A Invalid ID",pData->MyId);
#ifdef _DEBUG
				pView->ShowMessage(str);
#endif
			}			
			break;
		}
	case ONLINE:
		{
			int pIndex=pData->MyId-UserIdRadix;
			if(pIndex>=0&&pIndex<pApp->m_nTotalUserNumber)  // true means it esxit
			{
				if(pUsers[pIndex].State==0||pUsers[pIndex].State==2)
				{
					CData mc;
					mc.index=ONLINE_OK;
					mc.wIndex=pData->wIndex;
					mc.MyId=pData->MyId;
					mc.tarIP=pData->tarIP;
					mc.nPort=pData->nPort;
					if(!pSocket->SendData(&mc))break;
					
					if(pUsers[pIndex].State==0)
					{
						CRecordset recordset(pDatabase);
						CString mysql;
						mysql.Format("select FriendId from Friends where MyId=%d",pUsers[pIndex].Id);
						recordset.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
						CDBVariant value;
						CArray<DWORD,DWORD>pFriendId;
						while(!recordset.IsEOF())
						{
							recordset.GetFieldValue(short(0),value);
							pFriendId.Add(value.m_lVal);
							recordset.MoveNext();
						}
						recordset.Close();
						
						// 发送好友列表给用户
						CMsg2 msg;
						msg.index=ALL_FRIEND_ID;
						msg.wIndex=pData->wIndex;
						msg.tarIP=pData->tarIP;
						msg.nPort=pData->nPort;
						msg.MyId=pData->MyId;
						msg.aFriendId.Append(pFriendId);
						if(!pSocket->SendData(&msg))break;

						// 发送在线好友给用户
						CMsgOnlineFriend msg1;
						msg1.index=ONLINE_FRIEND;
						msg1.wIndex=pData->wIndex;
						msg1.tarIP=pData->tarIP;
						msg1.nPort=pData->nPort;
						msg1.MyId=pData->MyId;
						for(int i=0;i<pFriendId.GetSize();i++)
						{
							int pi=pFriendId.GetAt(i)-UserIdRadix;
							if(pUsers[pi].State)
							{
								msg1.aFriendId.Add(pUsers[pi].Id);
								msg1.aFriendIP.Add(pUsers[pi].IP);
								msg1.aFriendPort.Add(pUsers[pi].Port);
								msg1.aFriendState.Add(pUsers[pi].State);
							}
						}
						Sleep(500);
						if(!pSocket->SendData(&msg1))break;

						//offline broadcast
						mysql.Format("select broadcast.SendTime,broadcast.SenderId,broadcast.msg,offbroadcast.num from broadcast,offbroadcast where offbroadcast.msgid=broadcast.msgid and offbroadcast.RecvId=%d",pData->MyId);
						CRecordset rs(pDatabase);
						rs.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
						
						CMsg1 msg3;
						CDBVariant s1;
						msg3.wIndex=pData->wIndex;
						msg3.tarIP=pData->tarIP;
						msg3.nPort=pData->nPort;
						msg3.index=SYSTEM_BROADCAST;
						msg3.FriendId=pData->MyId;
						
						while(!rs.IsEOF())
						{
							rs.GetFieldValue(short(0),s1);
							msg3.Time=s1.m_lVal;
							rs.GetFieldValue(1,s1);
							msg3.MyId=s1.m_lVal;
							rs.GetFieldValue(2,msg3.Msg);
							rs.GetFieldValue(3,s1);									
							
							int inum=0;
							inum=s1.m_lVal;
							Sleep(500);
							if(pSocket->SendData(&msg3))
							{
								CString sql1;
								sql1.Format("delete from offbroadcast where num=%d",inum);
								pDatabase->ExecuteSQL(sql1);
							}
							else
							{
								rs.Close();
								goto proc_end;
							}
							rs.MoveNext();
						}
						rs.Close();					
					
						//offline msg
						mysql.Format("select * from offmsg where RecvId=%d",pData->MyId);
						rs.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
						
						CMsg1 offmsg3;
						CDBVariant dbMsgId,dbSenderId,dbRecvId,dbRecvTime,dbmIndex;
						offmsg3.tarIP=pData->tarIP;
						offmsg3.nPort=pData->nPort;							
						offmsg3.FriendId=pData->MyId;
						offmsg3.wIndex=pData->wIndex;
						
						while(!rs.IsEOF())
						{
							rs.GetFieldValue("MsgId",dbMsgId);
							rs.GetFieldValue("SenderId",dbSenderId);
							rs.GetFieldValue("RecvId",dbRecvId);
							rs.GetFieldValue("RecvTime",dbRecvTime);
							rs.GetFieldValue("mIndex",dbmIndex);
							rs.GetFieldValue("Msg",offmsg3.Msg);
							offmsg3.MyId=dbSenderId.m_lVal;
							offmsg3.Time=dbRecvTime.m_lVal;
							offmsg3.index=dbmIndex.m_chVal;								
							
							int inum=0;
							inum=dbMsgId.m_lVal;
							Sleep(500);
							if(pSocket->SendData(&offmsg3))
							{
								CString sql1;
								sql1.Format("delete from offmsg where MsgID=%d",inum);
								pDatabase->ExecuteSQL(sql1);
							}
							else
							{
								rs.Close();
								goto proc_end;
							}
							rs.MoveNext();
						}
						rs.Close();					
					
						::InterlockedIncrement((LPLONG)&pApp->m_nNumberOnline);
					}
					pUsers[pIndex].State=1;
					pUsers[pIndex].IP=pData->tarIP;
					pUsers[pIndex].Port=pData->nPort;					

					//找出把我当作好友的人,并发ONLINE消息
					CRecordset recordset(pDatabase);
					CString mysql;
					mysql.Format("select MyId from Friends where FriendId=%d",pUsers[pIndex].Id);
					recordset.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
				    CDBVariant value;
					CNoticeMsg msg;
					msg.index=SOMEONE_ONLINE;
					msg.MyId=pData->MyId;
					msg.IP=pData->tarIP;
					msg.Port=pData->nPort;
					while(!recordset.IsEOF())
					{
						recordset.GetFieldValue(short(0),value);
						int pi=value.m_lVal-UserIdRadix;
						if(pUsers[pi].State)
						{
							msg.tarIP=pUsers[pi].IP;
							msg.nPort=pUsers[pi].Port;
							pSocket->SendData(&msg);					
						}
						recordset.MoveNext();
					}
					recordset.Close();					
				}
				CString str;
				in_addr tIP;
				tIP.S_un.S_addr=pData->tarIP;
				str.Format("ID: %d 上线(正常登陆),IP: %s,Port: %d",pData->MyId,inet_ntoa(tIP),pData->nPort);				
#ifdef _DEBUG
				pView->ShowMessage(str);
#endif
			}
			else
			{
				CString str;
				str.Format("ID: %d A Invalid ID",pData->MyId);
#ifdef _DEBUG
				pView->ShowMessage(str);
#endif
			}			
			break;			
		}
	case OFFLINE:
		{
			int pIndex=pData->MyId-UserIdRadix;
			if(pIndex>=0&&pIndex<pApp->m_nTotalUserNumber)  // true means it esxit
			{
				if(pUsers[pIndex].State)
				{
					pUsers[pIndex].State=0;
					::InterlockedDecrement((LPLONG)&pApp->m_nNumberOnline);
					//找出把我当作好友的人,并发OFFLINE消息
					CRecordset recordset(pDatabase);
					CString mysql;
					mysql.Format("select MyId from Friends where FriendId=%d",pUsers[pIndex].Id);
					recordset.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
					CDBVariant value;
					CData msg;
					msg.index=SOMEONE_OFFLINE;
					msg.MyId=pData->MyId;					
					while(!recordset.IsEOF())
					{
						recordset.GetFieldValue(short(0),value);						
						int pi=value.m_lVal-UserIdRadix;
						if(pUsers[pi].State)
						{
							msg.tarIP=pUsers[pi].IP;
							msg.nPort=pUsers[pi].Port;
							pSocket->SendData(&msg);					
						}
						recordset.MoveNext();
					}
					recordset.Close();					
				}
				CString str;
				str.Format("ID: %d 下线",pData->MyId);
#ifdef _DEBUG
				pView->ShowMessage(str);
#endif							
			}
			else
			{
				CString str;
				str.Format("ID: %d A Invalid ID",pData->MyId);
#ifdef _DEBUG
				pView->ShowMessage(str);
#endif
			}			
			break;
		}
	case SEND_MSG_TO_FRIEND:
	case REFUSE_AS_FRIEND:
	case ACCEPT_AS_FRIEND:
	case FRIEND_IDENTITY_VALIDATE:
		{
			//  用户给朋友发的离线消息 
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())
			{
#ifdef _DEBUG
				pView->ShowMessage("msg.LoadFromBuf error!");
#endif				
				break;
			}
			
			int pIndex=msg.FriendId-UserIdRadix;
			int mIndex=msg.MyId-UserIdRadix;
			if(pIndex>=0&&pIndex<pApp->m_nTotalUserNumber&&mIndex>=0&&mIndex<pApp->m_nTotalUserNumber) //It's fit user
			{
				if(index==ACCEPT_AS_FRIEND)
				{
					CRecordset rs(pDatabase);
					CString sql1;
					sql1.Format("select friendid from friends where myid=%d and friendid=%d",msg.FriendId,msg.MyId);
					rs.Open(AFX_DB_USE_DEFAULT_TYPE,sql1);
					int count=rs.GetRecordCount();
					rs.Close();
					if(count==0)
					{
						CString mysql;
						mysql.Format("insert into Friends(Myid,FriendId) values(%d,%d)",msg.FriendId,msg.MyId);
						pDatabase->ExecuteSQL(mysql);
					}
					else break;
				}			
				
				// Is frined online?				
				BOOL SendOK=FALSE;
				if(pUsers[pIndex].State)
				{
					msg.tarIP=pUsers[pIndex].IP;
					msg.nPort=pUsers[pIndex].Port;
					if(pSocket->SendData(&msg))
						SendOK=TRUE;
				}
				// Save it to database
				if(!SendOK)
				{
					CString mysql;
					CTime t1=CTime::GetCurrentTime();
					
					mysql.Format("insert into offmsg(SenderId,RecvId,mIndex,RecvTime,Msg) values(%d,%d,%d,%d,'%s')",
						msg.MyId,msg.FriendId,msg.index,t1,msg.Msg);
					try
					{
						pDatabase->ExecuteSQL(mysql);
					}
					catch(CDBException e)
					{
#ifdef _DEBUG
						pView->ShowMessage(e.m_strError);
#endif
					}
				}
				CString str;
				str.Format("ID: %d To ID: %d 用户给朋友发的离线消息",msg.MyId,msg.FriendId);
#ifdef _DEBUG
				pView->ShowMessage(str);
#endif
			}
			else
			{
#ifdef _DEBUG
				pView->ShowMessage("A Invalid Id");
#endif
			}

			break;
		}
	case MULTI_SEND_MSG:
		{
			//  用户给多个朋友发的离线消息 
			CMsg2 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())
			{
				TRACE0("msg.LoadFromBuf error!\n");
				break;
			}
			
			int mIndex=msg.MyId-UserIdRadix;
			if(mIndex>=0&&mIndex<pApp->m_nTotalUserNumber) //It's fit user
			{
				for(int i=0;i<msg.aFriendId.GetSize();i++)
				{
					int pIndex=msg.aFriendId.GetAt(i)-UserIdRadix;
					if(pIndex<0||pIndex>=pApp->m_nTotalUserNumber)continue;
					
					// Is This frined online?				
					BOOL SendOK=FALSE;
					
					if(pUsers[pIndex].State)
					{
						CMsg1 msg1;
						msg1.index=SEND_MSG_TO_FRIEND;
						msg1.Msg=msg.Msg;
						msg1.MyId=msg.MyId;
						msg1.Time=msg.Time;
						msg1.FriendId=msg.aFriendId.GetAt(i);
						msg1.tarIP=pUsers[pIndex].IP;
						msg1.nPort=pUsers[pIndex].Port;
			
						if(pSocket->SendData(&msg1))
							SendOK=TRUE;
					}
					if(!SendOK)
					{						
						// Save it to database
						CString mysql;						
						CTime t1=CTime::GetCurrentTime();
						mysql.Format("insert into offmsg(SenderId,RecvId,mIndex,RecvTime,Msg) values(%d,%d,%d,%d,'%s')",
							msg.MyId,msg.aFriendId.GetAt(i),SEND_MSG_TO_FRIEND,t1,msg.Msg);
						try
						{
							pDatabase->ExecuteSQL(mysql);
						}
						catch(CDBException e)
						{
#ifdef _DEBUG
							pView->ShowMessage(e.m_strError);
#endif
						}												
					}
				}
				CString str;
				str.Format("ID: %d 用户给多个朋友发的离线消息",msg.MyId);
#ifdef _DEBUG
				pView->ShowMessage(str);
#endif
			}

			break;
		}
	case SEND_BROADCAST:
		{
			//  发送广播消息
			CMsg4 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())
			{
				TRACE0("msg.LoadFromBuf error!\n");
				break;
			}
			
			int mIndex=msg.MyId-UserIdRadix;
			if(mIndex>=0&&mIndex<pApp->m_nTotalUserNumber)
			{
				if(msg.BroadcastPwd==pApp->m_strBroadcastPwd)
				{
					CString mysql;
					CTime t1=CTime::GetCurrentTime();
					mysql.Format("insert into broadcast(SenderId,SendTime,Msg) values(%d,%d,'%s')",msg.MyId,t1,msg.Msg);
					pDatabase->ExecuteSQL(mysql);

					int CurMsgId=0;
					CRecordset rs(pDatabase);
					rs.Open(AFX_DB_USE_DEFAULT_TYPE,"select last(MsgId) from broadcast");
					CDBVariant v1;
					rs.GetFieldValue(short(0),v1);
					rs.Close();
					CurMsgId=v1.m_lVal;

					// Check for everyone ,send broadcast
					CMsg1 msg1;
					msg1.index=SYSTEM_BROADCAST;
					msg1.MyId=msg.MyId;
					msg1.Msg=msg.Msg;
					msg1.Time=t1;
					for(int i=0;i<pApp->m_nTotalUserNumber;i++)
					{
						if(pUsers[i].Id==0||pUsers[i].Id==msg.MyId)
							continue;
						BOOL SendOK=FALSE;
						if(pUsers[i].State)
						{
							msg1.tarIP=pUsers[i].IP;
							msg1.nPort=pUsers[i].Port;
							if(pSocket->SendData(&msg1))
								SendOK=TRUE;
						}
						if(!SendOK)
						{
							//Save it to Database
							CString mysql;
							mysql.Format("insert into OffBroadcast(RecvId,MsgId) values(%d,%d)",i+1+UserIdRadix,CurMsgId);
							pDatabase->ExecuteSQL(mysql);
						}
					}
					CString str;
					str.Format("ID: %d 发送广播消息",msg.MyId);
#ifdef _DEBUG
					pView->ShowMessage(str);
#endif
				}
			}

			break;
		}
	case FRIEND_DETAIL:
		{
			// 好友详细信息
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())
			{
				TRACE0("msg.LoadFromBuf error!\n");
				break;
			}
			TRACE1("Request Friend Detail %d\n",msg.FriendId);
			int mIndex=msg.FriendId-UserIdRadix;
			if(mIndex>=0&&mIndex<pApp->m_nTotalUserNumber&&pUsers[mIndex].Id)
			{				
				CMsgPerson msg1;
				msg1.index=RE_FRIEND_DETAIL;
				msg1.wIndex=msg.wIndex;
				msg1.tarIP=msg.tarIP;
				msg1.nPort=msg.nPort;
				msg1.MyId=pUsers[mIndex].Id;
				msg1.Name=pUsers[mIndex].Name;
				msg1.PhotoId=pUsers[mIndex].PhotoId;
				msg1.Department=pUsers[mIndex].Department;

				CRecordset recordset(pDatabase);
				CString mysql;
				mysql.Format("select Sex,Age,Email,Homepage,Address,Phone,fax,description,canbeadd from Users where Id=%d",msg.FriendId);
				recordset.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
				if(recordset.GetRecordCount()==0)break;
				CDBVariant s1;
				recordset.GetFieldValue(short(0),s1);
				msg1.Sex=s1.m_chVal;
				recordset.GetFieldValue(1,msg1.Age);				
				recordset.GetFieldValue(2,msg1.Email);				
				recordset.GetFieldValue(3,msg1.Homepage);				
				recordset.GetFieldValue(4,msg1.Address);				
				recordset.GetFieldValue(5,msg1.Phone);				
				recordset.GetFieldValue(6,msg1.Fax);				
				recordset.GetFieldValue(7,msg1.Description);				
				recordset.GetFieldValue(8,s1);
				msg1.Canbeadd=s1.m_chVal;

				recordset.Close();

				if(pSocket->SendData(&msg1))
				{
					CString str;
					str.Format("ID: %d FriendID: %d 好友详细信息",msg.MyId,msg.FriendId);
#ifdef _DEBUG
					pView->ShowMessage(str);
#endif
				}
			}
			
			break;
		}
	case ADD_AS_FRIEND:
		{	
			// 加为好友请求
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())
			{
				TRACE0("msg.LoadFromBuf error!\n");
				break;
			}

			int MyIndex=msg.MyId-UserIdRadix;
			int FriendIndex=msg.FriendId-UserIdRadix;

			//if it is not valid user.			
			if(MyIndex<0||MyIndex>=pApp->m_nTotalUserNumber||FriendIndex<0||FriendIndex>=pApp->m_nTotalUserNumber)
				break;			
			if(!pUsers[MyIndex].Id||!pUsers[FriendIndex].Id)break;

			// find the friendid user
			CMsg3 msg3;
			msg3.tarIP=msg.tarIP;
			msg3.nPort=msg.nPort;
			msg3.index=RE_ADD_AS_FRIEND;				
			msg3.wIndex=msg.wIndex;
			if(pUsers[FriendIndex].CanbeAdd!=1)
			{
				// 拒绝任何人加入 or 需要身份验证
				msg3.Value=pUsers[FriendIndex].CanbeAdd;
				if(!pSocket->SendData(&msg3))
					break;
			}
			else
			{
				// add it be my friend
				CRecordset rs(pDatabase);
				CString sql1;
				sql1.Format("select friendid from friends where myid=%d and friendid=%d",msg.MyId,msg.FriendId);
				rs.Open(AFX_DB_USE_DEFAULT_TYPE,sql1);
				int count=rs.GetRecordCount();
				rs.Close();
				if(msg.MyId!=msg.FriendId&&count==0)
				{
					CString mysql;
					mysql.Format("insert into Friends(Myid,FriendId) values(%d,%d)",msg.MyId,msg.FriendId);
					pDatabase->ExecuteSQL(mysql);
					msg3.Value=1;
					//成功加入
					pSocket->SendData(&msg3);
					
					//通知被加入的人											
					CMsg1 msg4;
					msg4.tarIP=pUsers[FriendIndex].IP;
					msg4.nPort=pUsers[FriendIndex].Port;
					msg4.index=BE_ADDED_AS_FRIEND;
					msg4.MyId=msg.MyId;
					msg4.FriendId=msg.FriendId;
					msg4.Msg.Format("%d 把你加为好友",msg.MyId);
					msg4.Time=CTime::GetCurrentTime();
					if(pUsers[FriendIndex].State&&pSocket->SendData(&msg4))
					{						
						Sleep(500);
						CNoticeMsg msg5;
						if(pUsers[FriendIndex].State==1)
							msg5.index=SOMEONE_ONLINE;
						else
							msg5.index=SOMEONE_ONHIDE;
						msg5.MyId=pUsers[FriendIndex].Id;
						msg5.IP=pUsers[FriendIndex].IP;
						msg5.Port=pUsers[FriendIndex].Port;
						msg5.tarIP=pData->tarIP;
						msg5.nPort=pData->nPort;
						pSocket->SendData(&msg5);
					}
					else
					{
						CString mysql;							
						
						mysql.Format("insert into offmsg(SenderId,RecvId,mIndex,RecvTime,Msg) values(%d,%d,%d,%d,'%s')",
							msg4.MyId,msg4.FriendId,msg4.index,msg4.Time,msg4.Msg);
						try
						{
							pDatabase->ExecuteSQL(mysql);
						}
						catch(CDBException e)
						{
#ifdef _DEBUG
							pView->ShowMessage(e.m_strError);
#endif
						}
					}
				}
				else
				{
					// this friend is already in your friends
					msg3.Value=3;
					if(!pSocket->SendData(&msg3))
						goto proc_end;
				}					
			}			

			CString str;
			str.Format("ID: %d want add ID: %d as friend!",msg.MyId,msg.FriendId);
#ifdef _DEBUG			
			pView->ShowMessage(str);			
#endif
			break;
		}
	case APPLY_SHOW_ONLINE:
		{
			//查看在线的人
			CMsg3 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
	
			int mIndex=msg.MyId-UserIdRadix;
			if(mIndex<0||mIndex>=pApp->m_nTotalUserNumber||!pUsers[mIndex].Id)break;

			int offsize=msg.Value;
			int count=0;
			int mOff=offsize*PersonNumEveryTime;
			if(mOff>=pApp->m_nTotalUserNumber) break;
			for(int i=0;i<pApp->m_nTotalUserNumber&&count<mOff;i++)
				if(pUsers[i].State)count++;
			if(count==mOff)
			{
				CShowOnlinePeople msg1;
				msg1.tarIP=msg.tarIP;
				msg1.nPort=msg.nPort;
				msg1.index=RECV_SHOW_ONLINE;
				msg1.wIndex=msg.wIndex;
				msg1.MyId=msg.MyId;
				for(int j=0;i<pApp->m_nTotalUserNumber&&j<PersonNumEveryTime;i++)
				{
					if(pUsers[i].State)
					{
						msg1.aPhotoId.Add(pUsers[i].PhotoId);
						msg1.aId.Add(pUsers[i].Id);
						msg1.aName.Add(pUsers[i].Name);
						msg1.aDepartment.Add(pUsers[i].Department);
						j++;
					}
				}
				if(msg1.aId.GetSize()>0)
					pSocket->SendData(&msg1);
			}

#ifdef _DEBUG
			pView->ShowMessage("查看在线的人");			
#endif
			break;
		}
	case FIND_FRIEND_BY_ID:		
		{
			//用号码查找朋友
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			int mIndex=msg.MyId-UserIdRadix;
			int fIndex=msg.FriendId-UserIdRadix;
			if(mIndex>=0&&mIndex<pApp->m_nTotalUserNumber&&fIndex>=0&&fIndex<pApp->m_nTotalUserNumber&&pUsers[fIndex].Id&&pUsers[mIndex].Id)  //fint it
			{
				//return this peron's detail
				CMsgPerson msg1;
				msg1.index=FOUND_FRIEND_BY_ID;
				msg1.wIndex=msg.wIndex;
				msg1.tarIP=msg.tarIP;
				msg1.nPort=msg.nPort;
				msg1.MyId=pUsers[fIndex].Id;
				msg1.Name=pUsers[fIndex].Name;
				msg1.PhotoId=pUsers[fIndex].PhotoId;
				msg1.Department=pUsers[fIndex].Department;

				CRecordset recordset(pDatabase);
				CString mysql;
				mysql.Format("select Sex,Age,Email,Homepage,Address,Phone,fax,description,canbeadd from Users where Id=%d",msg.FriendId);
				recordset.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
				if(recordset.GetRecordCount()==0)break;
				CDBVariant s1;
				recordset.GetFieldValue(short(0),s1);
				msg1.Sex=s1.m_chVal;
				recordset.GetFieldValue(1,msg1.Age);				
				recordset.GetFieldValue(2,msg1.Email);				
				recordset.GetFieldValue(3,msg1.Homepage);				
				recordset.GetFieldValue(4,msg1.Address);				
				recordset.GetFieldValue(5,msg1.Phone);				
				recordset.GetFieldValue(6,msg1.Fax);				
				recordset.GetFieldValue(7,msg1.Description);				
				recordset.GetFieldValue(8,s1);
				msg1.Canbeadd=s1.m_chVal;

				recordset.Close();
				pSocket->SendData(&msg1);							
			}
			else
			{
				//return not found!
				CData msg1;
				msg1.index=ID_NOT_FOUND_BY_ID;
				msg1.wIndex=msg.wIndex;
				msg1.tarIP=msg.tarIP;
				msg1.nPort=msg.nPort;
				msg1.MyId=msg.MyId;
				pSocket->SendData(&msg1);
			}
			//-----------------------
#ifdef _DEBUG	
			pView->ShowMessage("用号码查找朋友");			
#endif
			break;
		}
	case FIND_FRIEND_BY_NAME:
		{
			//用姓名查找朋友
			CMsg4 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
	
			int mIndex=msg.MyId-UserIdRadix;
			if(mIndex<0||mIndex>=pApp->m_nTotalUserNumber||!pUsers[mIndex].Id)break;

			CShowOnlinePeople msg1;			
			for(int i=0;i<pApp->m_nTotalUserNumber;i++)
			{
				if(pUsers[i].Name==msg.Msg)
				{
					msg1.aPhotoId.Add(pUsers[i].PhotoId);
					msg1.aId.Add(pUsers[i].Id);
					msg1.aName.Add(pUsers[i].Name);
					msg1.aDepartment.Add(pUsers[i].Department);					
				}
			}
			if(msg1.aId.GetSize()>0)
			{
				msg1.tarIP=msg.tarIP;
				msg1.nPort=msg.nPort;
				msg1.MyId=msg.MyId;
				msg1.index=FOUND_FRIEND_BY_NAME;
				msg1.wIndex=msg.wIndex;
				pSocket->SendData(&msg1);
			}
			else
			{
				CData msg2;
				msg2.tarIP=msg.tarIP;
				msg2.nPort=msg.nPort;
				msg2.MyId=msg.MyId;
				msg2.index=NAME_NOT_FOUND_BY_NAME;
				msg2.wIndex=msg.wIndex;
				pSocket->SendData(&msg2);
			}
					
#ifdef _DEBUG
			pView->ShowMessage("用姓名查找朋友");			
#endif
			break;
		}
	case DELETE_A_FRIEND:
		{
			// 删除一个好友
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
		
			int MyIndex=msg.MyId-UserIdRadix;
			int FriendIndex=msg.FriendId-UserIdRadix;

			//if it is not valid user.
			if(MyIndex<0||MyIndex>=pApp->m_nTotalUserNumber||FriendIndex<0||FriendIndex>=pApp->m_nTotalUserNumber)
				break;
			if(!pUsers[MyIndex].Id||!pUsers[FriendIndex].Id)break;
			
			// delete the friendid user				
			CString mysql;
			mysql.Format("delete from friends where myid=%d and friendid=%d",msg.MyId,msg.FriendId);
			pDatabase->ExecuteSQL(mysql);			

			CString str;
			str.Format("ID: %d want 删除一个好友ID: %d",msg.MyId,msg.FriendId);
#ifdef _DEBUG
			pView->ShowMessage(str);			
#endif
			break;			
		}
	case DELETE_SELF_IN_FRIEND:
		{
			//选择在某人的好友中删除自己
			CMsg1 msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;

			int MyIndex=msg.MyId-UserIdRadix;
			int FriendIndex=msg.FriendId-UserIdRadix;

			//if it is not valid user.
			if(MyIndex<0||MyIndex>=pApp->m_nTotalUserNumber||FriendIndex<0||FriendIndex>=pApp->m_nTotalUserNumber)
				break;
			if(!pUsers[MyIndex].Id||!pUsers[FriendIndex].Id)break;

			CString mysql;
			mysql.Format("delete from friends where myid=%d and friendid=%d",msg.FriendId,msg.MyId);
			pDatabase->ExecuteSQL(mysql);

#ifdef _DEBUG
			pView->ShowMessage("选择在某人的好友中删除自己");			
#endif
			break;
		}
	case CHANGE_PERSONAL_INFO:
		{
			CMsgChangePI msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			int mIndex=msg.MyId-UserIdRadix;
			if(mIndex<0||mIndex>=pApp->m_nTotalUserNumber||!pUsers[mIndex].Id)break;
			
			if(!msg.Mask)break;
			
			CString strHead,strWhere,str,Mysql;			
			strHead.Format("update users set ");
			strWhere.Format(" where id=%d",msg.MyId);
			
			if(msg.Mask&CMsgChangePI::NAME)
				str.Format("Name='%s',",msg.Name),strHead+=str;
			if(msg.Mask&CMsgChangePI::SEX)
				str.Format("Sex='%d',",msg.Sex),strHead+=str;
			if(msg.Mask&CMsgChangePI::AGE)
				str.Format("Age='%s',",msg.Age),strHead+=str;
			if(msg.Mask&CMsgChangePI::CANBEADD)
				str.Format("Canbeadd=%d,",msg.Canbeadd),strHead+=str;
			if(msg.Mask&CMsgChangePI::ADDRESS)
				str.Format("Address=%d,",msg.Address),strHead+=str;
			if(msg.Mask&CMsgChangePI::DEPARTMENT)
				str.Format("department='%s',",msg.Department),strHead+=str;
			if(msg.Mask&CMsgChangePI::DESCRIPTION)
				str.Format("Description='%s',",msg.Description),strHead+=str;
			if(msg.Mask&CMsgChangePI::EMAIL)
				str.Format("email='%s',",msg.Email),strHead+=str;
			if(msg.Mask&CMsgChangePI::FAX)
				str.Format("Fax='%s',",msg.Fax),strHead+=str;
			if(msg.Mask&CMsgChangePI::HOMEPAGE)
				str.Format("Homepage='%s',",msg.Homepage),strHead+=str;
			if(msg.Mask&CMsgChangePI::PHONE)
				str.Format("Phone='%s',",msg.Phone),strHead+=str;
			if(msg.Mask&CMsgChangePI::PHOTOID)
				str.Format("Photoid=%d,",msg.PhotoId),strHead+=str;
			
			if(strHead.GetAt(strHead.GetLength()-1)==',')
				Mysql=strHead.Left(strHead.GetLength()-1);
			else Mysql=strHead;
			
			Mysql+=strWhere;
			pDatabase->ExecuteSQL(Mysql);
			
			// change in pUsers array
			if(msg.Mask&CMsgChangePI::NAME)
				pUsers[mIndex].Name=msg.Name;
			if(msg.Mask&CMsgChangePI::CANBEADD)
				pUsers[mIndex].CanbeAdd=msg.Canbeadd;
			if(msg.Mask&CMsgChangePI::DEPARTMENT)
				pUsers[mIndex].Department=msg.Department;
			if(msg.Mask&CMsgChangePI::PHOTOID)
				pUsers[mIndex].PhotoId=msg.PhotoId;				
			
#ifdef _DEBUG
			pView->ShowMessage("CHANGE_PERSONAL_INFORMATION");			
#endif
			break;
		}
	case CHANGE_PASSWORD:
		{
			CMsgModifyPwd msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			int mIndex=msg.MyId-UserIdRadix;
			if(mIndex>=0&&mIndex<pApp->m_nTotalUserNumber&&pUsers[mIndex].Id)
			{
				CRecordset rs(pDatabase);
				CString mysql;
				mysql.Format("select Password from users where id=%d",msg.MyId);
				rs.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
				if(rs.GetRecordCount==0)break;
				CString pwd;
				rs.GetFieldValue(short(0),pwd);
				if(msg.OldPwd==pwd)
				{
					CString sql1;
					sql1.Format("update users set Password=%s where id=%d",msg.NewPwd,msg.MyId);
					pDatabase->ExecuteSQL(sql1);
				}
			}		
			
			CString str;
			str.Format("ID: %d 修改个人信息",msg.MyId);
#ifdef _DEBUG
			pView->ShowMessage(str);
#endif
			break;
		}
	case APPLY_ID_LOGIN:
		{
			CMsgPerson msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;		
			
			CString mysql; 
			int nAppID=0;

			pApp->m_criticalMaxUserId.Lock();
			pApp->m_nTotalUserNumber++;
			pApp->m_nMaxUserId++;
			nAppID=pApp->m_nMaxUserId;
			pApp->m_criticalMaxUserId.Unlock();

			mysql.Format("insert into users(id,photoid,password,name,sex,age,canbeadd,email,homepage,address,phone,fax,department,description) values(%d,%d,'%s','%s',%d,'%s',%d,'%s','%s','%s','%s','%s','%s','%s')",
				nAppID,msg.PhotoId,msg.Password,msg.Name,msg.Sex,msg.Age,msg.Canbeadd,msg.Email,msg.Homepage,msg.Address,msg.Phone,msg.Fax,msg.Department,msg.Description);
			try
			{
				pDatabase->ExecuteSQL(mysql);
			}
			catch(CDBException e)
			{
				AfxMessageBox(e.m_strError);
			}
			
			CData msg1;
			msg1.index=APPLY_ID_OK;
			msg1.MyId=nAppID;
			msg1.tarIP=msg.tarIP;
			msg1.nPort=msg.nPort;
			pSocket->SendData(&msg1);			
			
			::InterlockedIncrement((LPLONG)&pApp->m_nNumberOnline);

			pUsers[msg1.MyId-UserIdRadix].Id=msg1.MyId;
			pUsers[msg1.MyId-UserIdRadix].Name=msg.Name;
			pUsers[msg1.MyId-UserIdRadix].CanbeAdd=msg.Canbeadd;
			pUsers[msg1.MyId-UserIdRadix].PhotoId=msg.PhotoId;
			pUsers[msg1.MyId-UserIdRadix].Department=msg.Department;
			pUsers[msg1.MyId-UserIdRadix].IP=msg.tarIP;
			pUsers[msg1.MyId-UserIdRadix].Port=msg.nPort;
			pUsers[msg1.MyId-UserIdRadix].State=1;				
			
			CString str;
			str.Format("用户：%s 申请帐号成功，ID：%d",msg.Name,msg1.MyId);
#ifdef _DEBUG
			pView->ShowMessage(str);			
#endif
			
			break;
		}
	case HAVE_ID_LOGIN:
		{
			//使用已有号码进行登陆
			CMsgModifyPwd msg;
			CopyDataBuf(&msg,pData);
			if(!msg.LoadFromBuf())break;
			
			int mIndex=msg.MyId-UserIdRadix;
			
			CMsg3 msg1;
			msg1.index=RE_LOGIN_INFO;
			msg1.wIndex=msg.wIndex;
			msg1.MyId=msg.MyId;
			msg1.tarIP=msg.tarIP;
			msg1.nPort=msg.nPort;
			
			if(mIndex>=0&&mIndex<pApp->m_nTotalUserNumber&&pUsers[mIndex].Id)			
			{
				CRecordset rs(pDatabase);
				CString mysql;
				mysql.Format("select Password from users where id=%d",msg.MyId);
				rs.Open(AFX_DB_USE_DEFAULT_TYPE,mysql);
				if(rs.GetRecordCount==0)break;
				CString pwd;
				rs.GetFieldValue(short(0),pwd);				
				
				if(msg.OldPwd!=pwd)
				{
					msg1.Value=1;
					pSocket->SendData(&msg1);
				}
				else
				{
					msg1.Value=2;
					pSocket->SendData(&msg1);
				}
			}
			else
			{
				msg1.Value=0;
				pSocket->SendData(&msg1);
			}
			
			CString str;
			str.Format("ID: %d 使用已有ID进行登陆!",msg.MyId);
#ifdef _DEBUG
			pView->ShowMessage(str);
#endif
			break;
		}		
	}
proc_end:
	if(pData)
	{
		delete pData;
		pData=NULL;
	}

	return 0;
}