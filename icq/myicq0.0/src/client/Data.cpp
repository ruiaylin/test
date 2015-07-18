// Data.cpp: implementation of the CData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Data.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CData::CData()
{
	memset(szBuf,0,sizeof(szBuf));
	num=0;	
	tarIP=0;
	nPort=0;
	wIndex=0;
}

CData::~CData()
{

}
void CData::PackToBuf()
{
	BP=0;
	szBuf[BP++]=index;
	*((DWORD*)(szBuf+BP))=MyId;
	BP+=sizeof(DWORD);
	*((DWORD*)(szBuf+BP))=This;
	BP+=sizeof(DWORD);
	*(szBuf+BP)=wIndex;
	BP+=sizeof(char);
	num=BP;
}
BOOL CData::LoadFromBuf()
{
	BP=0;
	index=szBuf[BP++];
	MyId=*((DWORD*)(szBuf+BP));
	BP+=sizeof(DWORD);
	This=*((DWORD*)(szBuf+BP));
	BP+=sizeof(DWORD);
	wIndex=*(szBuf+BP);
	BP+=sizeof(char);
	if(BP<=num)	return TRUE;
	return FALSE;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

