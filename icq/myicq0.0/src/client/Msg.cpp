// Msg.cpp: implementation of the CMsg1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "common.h"
#include "Msg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsg1::CMsg1()
{
	Msg="";
}

CMsg1::~CMsg1()
{

}

void CMsg1::PackToBuf()
{
	CData::PackToBuf();

	*((DWORD*)(szBuf+BP))=FriendId;
	BP+=sizeof(DWORD);

	*((CTime*)(szBuf+BP))=Time;
	BP+=sizeof(CTime);

	WORD sLen=Msg.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	

	if(sLen>0)
	{
		strcpy(szBuf+BP,Msg.GetBuffer(Msg.GetLength()));
		BP+=sLen;
	}

	num=BP;
}

BOOL CMsg1::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	FriendId=*((DWORD*)(szBuf+BP));
	BP+=sizeof(DWORD);

	Time=*((CTime*)(szBuf+BP));
	BP+=sizeof(CTime);

	WORD sLen=0;
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);

	if(sLen>0&&BP+sLen<=num)
	{
		CString msg(szBuf+BP,sLen);
		Msg=msg;
		BP+=sLen;
	}	
	else if(sLen==0) 
	{
		Msg="";
	}
	else
	{
		return FALSE;
	}
	
	if(BP==num)return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMsg2 Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsg2::CMsg2()
{
	Msg="";
}

CMsg2::~CMsg2()
{

}

void CMsg2::PackToBuf()
{
	CData::PackToBuf();

	WORD aNum=0;
	aNum=aFriendId.GetSize();
	*((WORD*)(szBuf+BP))=aNum;
	BP+=sizeof(WORD);	

	for(int i=0;i<aNum;i++)
	{		
		*((DWORD*)(szBuf+BP))=aFriendId.GetAt(i);
		BP+=sizeof(DWORD);
	}

	*((CTime*)(szBuf+BP))=Time;
	BP+=sizeof(CTime);

	WORD sLen=Msg.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Msg.GetBuffer(Msg.GetLength()));
		BP+=sLen;
	}

	num=BP;
}

BOOL CMsg2::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	WORD aNum=0;
	aNum=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);

	if(aNum>0)
	{
		aFriendId.RemoveAll();
		for(int i=0;i<aNum;i++)
		{
			aFriendId.Add(*((DWORD*)(szBuf+BP)));
			BP+=sizeof(DWORD);
		}
	}
	else if(aNum<0)
	{
		return FALSE;
	}

	Time=*((CTime*)(szBuf+BP));
	BP+=sizeof(CTime);

	WORD sLen=0;
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);

	if(sLen>0&&BP+sLen<=num)
	{
		CString msg(szBuf+BP,sLen);
		Msg=msg;
		BP+=sLen;
	}	
	else if(sLen==0) 
	{
		Msg="";
	}
	else
	{
		return FALSE;
	}
	
	if(BP==num)return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMsg3 Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsg3::CMsg3()
{

}

CMsg3::~CMsg3()
{

}

void CMsg3::PackToBuf()
{
	CData::PackToBuf();

	*((DWORD*)(szBuf+BP))=Value;
	BP+=sizeof(DWORD);

	num=BP;
}

BOOL CMsg3::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	Value=*((DWORD*)(szBuf+BP));
	BP+=sizeof(DWORD);
	
	if(BP==num)return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMsg4 Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsg4::CMsg4()
{
	Msg="";
	BroadcastPwd="";
}

CMsg4::~CMsg4()
{

}

void CMsg4::PackToBuf()
{
	CData::PackToBuf();
	
	WORD sLen=BroadcastPwd.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,BroadcastPwd.GetBuffer(BroadcastPwd.GetLength()));
		BP+=sLen;
	}

	sLen=Msg.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Msg.GetBuffer(Msg.GetLength()));
		BP+=sLen;
	}

	num=BP;
}

BOOL CMsg4::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	WORD sLen=0;
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);
	if(sLen>0&&BP+sLen<=num)
	{
		CString msg(szBuf+BP,sLen);
		BroadcastPwd=msg;
		BP+=sLen;
	}	
	else if(sLen==0) 
	{
		BroadcastPwd="";
	}
	else
	{
		return FALSE;
	}
	
	sLen=0;
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);
	if(sLen>0&&BP+sLen<=num)
	{
		CString msg(szBuf+BP,sLen);
		Msg=msg;
		BP+=sLen;
	}	
	else if(sLen==0) 
	{
		Msg="";
	}
	else
	{
		return FALSE;
	}

	if(BP==num)return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMsgModifyPwd Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgModifyPwd::CMsgModifyPwd()
{

}

CMsgModifyPwd::~CMsgModifyPwd()
{

}

void CMsgModifyPwd::PackToBuf()
{
	CData::PackToBuf();
	
	WORD sLen=OldPwd.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,OldPwd.GetBuffer(OldPwd.GetLength()));
		BP+=sLen;
	}

	sLen=NewPwd.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,NewPwd.GetBuffer(NewPwd.GetLength()));
		BP+=sLen;
	}

	num=BP;	
}

BOOL CMsgModifyPwd::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	WORD sLen=0;
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);
	if(sLen>0&&BP+sLen<=num)
	{
		CString msg(szBuf+BP,sLen);
		OldPwd=msg;
		BP+=sLen;
	}	
	else if(sLen==0) 
	{
		OldPwd="";
	}
	else
	{
		return FALSE;
	}
	
	sLen=0;
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);
	if(sLen>0&&BP+sLen<=num)
	{
		CString msg(szBuf+BP,sLen);
		NewPwd=msg;
		BP+=sLen;
	}	
	else if(sLen==0) 
	{
		NewPwd="";
	}
	else
	{
		return FALSE;
	}

	if(BP==num)return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMsgOnlineFriend Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgOnlineFriend::CMsgOnlineFriend()
{

}

CMsgOnlineFriend::~CMsgOnlineFriend()
{

}

BOOL CMsgOnlineFriend::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	WORD aNum=0;
	aNum=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);
	
	if(aNum>0)
	{
		aFriendId.RemoveAll();
		aFriendState.RemoveAll();
		aFriendIP.RemoveAll();
		aFriendPort.RemoveAll();
		for(int i=0;i<aNum;i++)
		{
			aFriendId.Add(*((DWORD*)(szBuf+BP)));
			BP+=sizeof(DWORD);
			aFriendState.Add(*(szBuf+BP));
			BP++;
			aFriendIP.Add(*((DWORD*)(szBuf+BP)));
			BP+=sizeof(DWORD);
			aFriendPort.Add(*((DWORD*)(szBuf+BP)));
			BP+=sizeof(DWORD);
		}
	}
	else if(aNum<0) return FALSE;

	if(BP==num)return TRUE;
	return FALSE;
}

void CMsgOnlineFriend::PackToBuf()
{
	CData::PackToBuf();

	WORD aNum=aFriendId.GetSize();
	
	*((WORD*)(szBuf+BP))=aNum;
	BP+=sizeof(WORD);
	for(int i=0;i<aNum;i++)
	{
		*((DWORD*)(szBuf+BP))=aFriendId.GetAt(i);
		BP+=sizeof(DWORD);
		*(szBuf+BP)=aFriendState.GetAt(i);
		BP++;
		*((DWORD*)(szBuf+BP))=aFriendIP.GetAt(i);
		BP+=sizeof(DWORD);
		*((DWORD*)(szBuf+BP))=aFriendPort.GetAt(i);
		BP+=sizeof(DWORD);
	}
	num=BP;
}

//////////////////////////////////////////////////////////////////////
// CMsgChangePI Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgChangePI::CMsgChangePI()
{
	Mask=0;	
}

CMsgChangePI::~CMsgChangePI()
{

}

void CMsgChangePI::PackToBuf()
{
	CData::PackToBuf();

	*((DWORD*)(szBuf+BP))=Mask;
	BP+=sizeof(DWORD);

	if(Mask&NAME)
	{
		WORD sLen=Name.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Name.GetBuffer(Name.GetLength()));
			BP+=sLen;
		}	
	}

	if(Mask&SEX)
	{
		*((BYTE*)(szBuf+BP))=Sex;
		BP++;
	}

	if(Mask&CANBEADD)
	{		
		*((BYTE*)(szBuf+BP))=Canbeadd;
		BP++;
	}

	if(Mask&PHOTOID)
	{
		*((DWORD*)(szBuf+BP))=PhotoId;
		BP+=sizeof(DWORD);
	}

	if(Mask&AGE)
	{
		WORD sLen=Age.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Age.GetBuffer(Age.GetLength()));
			BP+=sLen;
		}	
	}

	if(Mask&PHONE)
	{
		WORD sLen=Phone.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Phone.GetBuffer(Phone.GetLength()));
			BP+=sLen;
		}
	}

	if(Mask&FAX)
	{
		WORD sLen=Fax.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Fax.GetBuffer(Fax.GetLength()));
			BP+=sLen;
		}		
	}

	if(Mask&EMAIL)
	{
		WORD sLen=Email.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Email.GetBuffer(Email.GetLength()));
			BP+=sLen;
		}
	}

	if(Mask&HOMEPAGE)
	{
		WORD sLen=Homepage.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Homepage.GetBuffer(Homepage.GetLength()));
			BP+=sLen;
		}
	}

	if(Mask&ADDRESS)
	{
		WORD sLen=Address.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Address.GetBuffer(Address.GetLength()));
			BP+=sLen;
		}
	}

	if(Mask&DEPARTMENT)
	{
		WORD sLen=Department.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Department.GetBuffer(Department.GetLength()));
			BP+=sLen;
		}
	}

	if(Mask&DESCRIPTION)
	{
		WORD sLen=Description.GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,Description.GetBuffer(Description.GetLength()));
			BP+=sLen;
		}
	}
	num=BP;
}

BOOL CMsgChangePI::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	Mask=*((DWORD*)(szBuf+BP));
	BP+=sizeof(DWORD);

	if(Mask&NAME)
	{
		WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Name=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(Mask&SEX)
	{
		Sex=*((BYTE*)(szBuf+BP));
		BP++;
	}

	if(Mask&CANBEADD)
	{		
		Canbeadd=*((BYTE*)(szBuf+BP));
		BP++;
	}

	if(Mask&PHOTOID)
	{
		PhotoId=*((DWORD*)(szBuf+BP));
		BP+=sizeof(DWORD);
	}

	if(Mask&AGE)
	{
		WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Age=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(Mask&PHONE)
	{
		WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Phone=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(Mask&FAX)
	{
		WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Fax=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(Mask&EMAIL)
	{
		WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Email=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(Mask&HOMEPAGE)
	{
		WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Homepage=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(Mask&ADDRESS)
	{
		WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Address=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(Mask&DEPARTMENT)
	{
		WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Department=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(Mask&DESCRIPTION)
	{
	WORD sLen;
		sLen=*((WORD*)(szBuf+BP));
		BP+=sizeof(WORD);	
		
		if(sLen>0&&BP+sLen<=num)
		{
			CString a(szBuf+BP,sLen);
			Description=a;			
			BP+=sLen;
		}
		else if(sLen<0) return FALSE;
	}

	if(num==BP)return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMsgPerson Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgPerson::CMsgPerson()
{

}

CMsgPerson::~CMsgPerson()
{

}

void CMsgPerson::PackToBuf()
{
	CData::PackToBuf();
	
	WORD sLen=Name.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Name.GetBuffer(Name.GetLength()));
		BP+=sLen;
	}	

	*((BYTE*)(szBuf+BP))=Sex;
	BP++;
	
	*((BYTE*)(szBuf+BP))=Canbeadd;
	BP++;
	
	*((DWORD*)(szBuf+BP))=PhotoId;
	BP+=sizeof(DWORD);
	
	sLen=Age.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Age.GetBuffer(Age.GetLength()));
		BP+=sLen;
	}	
	sLen=Phone.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Phone.GetBuffer(Phone.GetLength()));
		BP+=sLen;
	}	

	sLen=Fax.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Fax.GetBuffer(Fax.GetLength()));
		BP+=sLen;
	}		

	sLen=Email.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Email.GetBuffer(Email.GetLength()));
		BP+=sLen;
	}
	
	sLen=Homepage.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Homepage.GetBuffer(Homepage.GetLength()));
		BP+=sLen;
	}

	sLen=Address.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Address.GetBuffer(Address.GetLength()));
		BP+=sLen;
	}

	sLen=Department.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Department.GetBuffer(Department.GetLength()));
		BP+=sLen;
	}

	sLen=Description.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Description.GetBuffer(Description.GetLength()));
		BP+=sLen;
	}
	
	sLen=Password.GetLength();
	*((WORD*)(szBuf+BP))=sLen;
	BP+=sizeof(WORD);	
	
	if(sLen>0)
	{
		strcpy(szBuf+BP,Password.GetBuffer(Password.GetLength()));
		BP+=sLen;
	}

	num=BP;
}

BOOL CMsgPerson::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;
	
	WORD sLen;
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Name=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;
	
	
	Sex=*((BYTE*)(szBuf+BP));
	BP++;
		
	Canbeadd=*((BYTE*)(szBuf+BP));
	BP++;
	
	PhotoId=*((DWORD*)(szBuf+BP));
	BP+=sizeof(DWORD);
	
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Age=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;	
	
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Phone=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;	
	
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Fax=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;
	
	
	sLen;
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Email=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;
	
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Homepage=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;	
	
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Address=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;	
	
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Department=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;	
	
	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Description=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;

	sLen=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);	
	
	if(sLen>0&&BP+sLen<=num)
	{
		CString a(szBuf+BP,sLen);
		Password=a;			
		BP+=sLen;
	}
	else if(sLen<0) return FALSE;
	
	if(num==BP)return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CShowOnlinePeople Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShowOnlinePeople::CShowOnlinePeople()
{

}

CShowOnlinePeople::~CShowOnlinePeople()
{

}

void CShowOnlinePeople::PackToBuf()
{
	CData::PackToBuf();
	WORD aNum=aId.GetSize();
	
	*((WORD*)(szBuf+BP))=aNum;
	BP+=sizeof(WORD);
	for(int i=0;i<aNum;i++)
	{
		*((DWORD*)(szBuf+BP))=aPhotoId.GetAt(i);
		BP+=sizeof(DWORD);
		*((DWORD*)(szBuf+BP))=aId.GetAt(i);
		BP+=sizeof(DWORD);
		
		WORD sLen=aName[i].GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,aName[i].GetBuffer(aName[i].GetLength()));
			BP+=sLen;
		}

		sLen=aDepartment[i].GetLength();
		*((WORD*)(szBuf+BP))=sLen;
		BP+=sizeof(WORD);	
		
		if(sLen>0)
		{
			strcpy(szBuf+BP,aDepartment[i].GetBuffer(aDepartment[i].GetLength()));
			BP+=sLen;
		}
	}

	num=BP;
}

BOOL CShowOnlinePeople::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	WORD aNum=0;
	aNum=*((WORD*)(szBuf+BP));
	BP+=sizeof(WORD);
	
	if(aNum>0)
	{		
		aPhotoId.RemoveAll();
		aId.RemoveAll();
		aName.RemoveAll();
		aDepartment.RemoveAll();
		for(int i=0;i<aNum;i++)
		{
			aPhotoId.Add(*((DWORD*)(szBuf+BP)));
			BP+=sizeof(DWORD);			
			aId.Add(*((DWORD*)(szBuf+BP)));
			BP+=sizeof(DWORD);

			WORD sLen;
			sLen=*((WORD*)(szBuf+BP));
			BP+=sizeof(WORD);	
			
			if(sLen>=0&&sLen<num)
			{
				CString a(szBuf+BP,sLen);
				aName.Add(a);
				BP+=sLen;
			}
			else if(sLen!=0) return FALSE;

			sLen=*((WORD*)(szBuf+BP));
			BP+=sizeof(WORD);	
			
			if(sLen>=0&&sLen<num)
			{
				CString a(szBuf+BP,sLen);
				aDepartment.Add(a);
				BP+=sLen;
			}
			else if(sLen!=0) return FALSE;
		}
	}
	else if(aNum<0)return FALSE;

	if(BP==num)return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CNoticeMsg Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNoticeMsg::CNoticeMsg()
{

}

CNoticeMsg::~CNoticeMsg()
{

}

BOOL CNoticeMsg::LoadFromBuf()
{
	if(!CData::LoadFromBuf())return FALSE;

	IP=*((DWORD*)(szBuf+BP));
	BP+=sizeof(DWORD);
	Port=*((DWORD*)(szBuf+BP));
	BP+=sizeof(DWORD);

	if(BP==num)return TRUE;
	return FALSE;
}

void CNoticeMsg::PackToBuf()
{
	CData::PackToBuf();
	*((DWORD*)(szBuf+BP))=IP;
	BP+=sizeof(DWORD);
	*((DWORD*)(szBuf+BP))=Port;
	BP+=sizeof(DWORD);
	num=BP;
}
