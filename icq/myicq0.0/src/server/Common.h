#ifndef _COMMON_HEADER
#define _COMMON_HEADER

#define ResponseMsg "ok"
#define SETUPFILE "config.ini"
#define DEFAULT_SEND_NUM  5
#define DEFAULT_DATAS_PWD ""
#define DEFAULT_DSN    "IDServer"  
#define DEFAULT_UID  ""
#define DEFAULT_BROADCAST_PWD "1234567890"
#define DEFAULT_RECV_PORT_STR "4000,4001,4002"
const int ListenSocketNum=10;
const int SendSocketNum=15;
const int TimeWaitForRes=6000;
const int MaxUserNumber=500;
const int UserIdRadix=2000;
const int PersonNumEveryTime=20;
const int TimerSpanServer=30000;
const int TimerSpanClient=40000;
const int DataBufLength=1000;
const int MaxResponseMsgLength=20;
const int CheckBufLength=512;

typedef struct{
	BYTE State;  //0 offline,1 online,2 onhide
	BYTE CanbeAdd;
	DWORD Id;
	DWORD IP;
	DWORD Port;
	DWORD PhotoId;
	CString Name;
	CString Department;
}UserOnline;

UINT ProcessRecvData(LPVOID param);
UINT CheckOnline(LPVOID param);

//--------------------------- Server Receive ------------------------------------------------
//--------------------------- Client Send To Server------------------------------------------
#define SEND_MSG_TO_FRIEND       1   //CMsg1 index,friendId,myId,msg,time  
#define FRIEND_IDENTITY_VALIDATE 2   //CMsg1
#define ADD_AS_FRIEND            3   //CMsg1 
#define FRIEND_DETAIL            4   //CMsg1 
#define FIND_FRIEND_BY_ID        5   //CMsg1 
#define DELETE_A_FRIEND          6   //CMsg1 
#define DELETE_SELF_IN_FRIEND    7   //CMsg1  选择在某人的好友中删除自己
#define ACCEPT_AS_FRIEND         8   //CMsg1  响应加为好友请求反馈信息(属于系统消息)
#define REFUSE_AS_FRIEND         9   //CMsg1  响应加为好友请求反馈信息(属于系统消息)

#define ONLINE                  10   //CData index,myid 
#define ONHIDE                  11   //CData index,myid 
#define OFFLINE                 12   //CData index,myid 

#define MULTI_SEND_MSG          13   //CMsg2 index,myid,friendidarray,msg,time  

#define APPLY_SHOW_ONLINE       14   //CMsg3  index,myid,value 请求查看在线的人 

#define TEST_BROADCAST_PWD      15   //CMsg4 index,id,broadcastpwd,msg  
#define SEND_BROADCAST          16   //CMsg4 
#define FIND_FRIEND_BY_NAME     17   //CMsg4 index,myid，name(msg)

#define CHANGE_PERSONAL_INFO    18   //CMsgModifyPI index,myid,mask,name,address,phone,fax,email,homepage,photoid,canbeadd,department,sex,age,description

#define CHANGE_PASSWORD         19   //CMsgModifyPwd index,myid,oldpwd,newpwd 
#define HAVE_ID_LOGIN           20   //CMsgModifyPwd index,id,pwd(oldpwd) 

#define APPLY_ID_LOGIN          21   //CMsgPerson index,name,address,phone,fax,email,homepage,photoid,canbeadd,department,sex,age,pwd,description

//-------------------------------- Server Send To Client -------------------------------------------
//-------------------------------- Client Recv From Server -----------------------------------------
#define RE_ADD_AS_FRIEND         31   //CMsg3 index,myid,value(0,1,2,3)(别人拒绝，成功加入，要求验证人份，此人已经为好友)
#define RE_TEST_BROADCAST_PWD    32   //CMsg3 index,myid,value (0,1) 密码错误，密码正确 
#define TOTAL_ONLINE             33   //CMsg3 index,myId,Value 响应发送的总在线人数信息 
#define RE_LOGIN_INFO            34   //CMsg3 index,myid,value (0,1,2) 帐号不存在,密码错误,OK 

#define RECV_SHOW_ONLINE         35   //CShowOnlinePeople
#define FOUND_FRIEND_BY_NAME     36   //CShowOnlinePeople

#define APPLY_ID_OK              37   //CData index,myid(收到这个信息，这表示登陆成功)
#define ID_NOT_FOUND_BY_ID       38   //CData
#define NAME_NOT_FOUND_BY_NAME   39   //CData
#define REQUIRE_IDENTITY_VALIDATE 40  //CData 
#define ONHIDE_OK                41
#define ONLINE_OK                52

#define FOUND_FRIEND_BY_ID       43   //CMsgPerson 
#define RE_FRIEND_DETAIL         44   //CMsgPerson

#define ALL_FRIEND_ID            45   //CMsg2 index,id,friendidarray(收到这个信息，这表示登陆成功)

#define ONLINE_FRIEND            46   //CMsgOnlineFriend index,id,aFriendid,aFriendstate,aFriendIP,aFriendPort

#define BE_ADDED_AS_FRIEND       47   //CMsg1 响应好友加入的消息
#define SYSTEM_BROADCAST         48   //CMsg1
#define SOMEONE_ONLINE           49
#define SOMEONE_ONHIDE           50
#define SOMEONE_OFFLINE          51
//----------------------------------Client To Client-----------------------------------------------
//SEND_MSG_TO_FRIEND
//FRIEND_IDENTITY_VALIDATE

#endif