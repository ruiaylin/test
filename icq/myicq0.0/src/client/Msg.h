// Msg.h: interface for the CMsg1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSG_H__19113F14_4795_11D5_B10A_ECBDE0D3F600__INCLUDED_)
#define AFX_MSG_H__19113F14_4795_11D5_B10A_ECBDE0D3F600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Data.h"

class CMsg1 : public CData  
{
public:
	virtual BOOL LoadFromBuf();
	virtual void PackToBuf();
	CMsg1();
	virtual ~CMsg1();
	DWORD FriendId;
	CTime Time;
	CString Msg;
};


class CMsg2 : public CData  
{
public:
	virtual BOOL LoadFromBuf();
	virtual void PackToBuf();
	CMsg2();
	virtual ~CMsg2();
	CArray<DWORD,DWORD> aFriendId;
	CTime Time;
	CString Msg;	
};


class CMsg3 : public CData  
{
public:
	virtual BOOL LoadFromBuf();
	virtual void PackToBuf();
	CMsg3();
	virtual ~CMsg3();
	DWORD Value;

};


class CMsg4 : public CData  
{
public:
	virtual BOOL LoadFromBuf();
	virtual void PackToBuf();
	CMsg4();
	virtual ~CMsg4();
    CString BroadcastPwd;
	CString Msg;
};


class CMsgModifyPwd : public CData  
{
public:
	virtual BOOL LoadFromBuf();
	virtual void PackToBuf();
	CMsgModifyPwd();
	virtual ~CMsgModifyPwd();
	CString OldPwd;
	CString NewPwd;
};


class CMsgOnlineFriend : public CData  
{
public:
	virtual void PackToBuf();
	virtual BOOL LoadFromBuf();
	CMsgOnlineFriend();
	virtual ~CMsgOnlineFriend();

	CArray<DWORD,DWORD> aFriendId;
	CArray<BYTE,BYTE>  aFriendState;
	CArray<DWORD,DWORD> aFriendIP;
	CArray<DWORD,DWORD> aFriendPort;
};


class CMsgChangePI : public CData  
{
public:
	enum{NAME=1<<0,SEX=1<<1,AGE=1<<2,ADDRESS= 1<<3,PHONE= 1<<4,
		FAX=1<<5,EMAIL=1<<6,HOMEPAGE=1<<7,PHOTOID=1<<8,CANBEADD=1<<9,
		DEPARTMENT=1<<10,DESCRIPTION=1<<11};

public:
	virtual BOOL LoadFromBuf();
	virtual void PackToBuf();
	CMsgChangePI();
	virtual ~CMsgChangePI();
	DWORD Mask;
	CString Name;
	BYTE Sex;
	BYTE Canbeadd;
	DWORD PhotoId;	
	CString Age;	
	CString Phone;
	CString Fax;
	CString Email;
	CString Homepage;
	CString Address;	
	CString Department;
	CString Description;
};


class CMsgPerson : public CData  
{
public:
	virtual BOOL LoadFromBuf();
	virtual void PackToBuf();
	CMsgPerson();
	virtual ~CMsgPerson();
	CString Name;
	BYTE Sex;
	BYTE Canbeadd;
	DWORD PhotoId;	
	CString Age;	
	CString Phone;
	CString Fax;
	CString Email;
	CString Homepage;
	CString Address;	
	CString Department;
	CString Description;
	CString Password;

};


class CShowOnlinePeople : public CData  
{
public:
	virtual BOOL LoadFromBuf();
	virtual void PackToBuf();
	CShowOnlinePeople();
	virtual ~CShowOnlinePeople();
	CArray<DWORD,DWORD>aPhotoId;
	CArray<DWORD,DWORD>aId;
	CArray<CString,CString>aName;
	CArray<CString,CString>aDepartment;	
};


class CNoticeMsg : public CData  
{
public:
	virtual void PackToBuf();
	virtual BOOL LoadFromBuf();
	CNoticeMsg();
	virtual ~CNoticeMsg();
	DWORD IP;
	DWORD Port;
};

#endif // !defined(AFX_MSG_H__19113F14_4795_11D5_B10A_ECBDE0D3F600__INCLUDED_)
