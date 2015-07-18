// Data.h: interface for the CData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATA_H__C0A9FB14_44DC_11D5_A7F9_00E04C41C3AD__INCLUDED_)
#define AFX_DATA_H__C0A9FB14_44DC_11D5_A7F9_00E04C41C3AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"

class CData  
{
public:
	CData();
	virtual ~CData();
	char szBuf[DataBufLength];
	int num;
	BYTE index;
	DWORD MyId;
	DWORD This;
	BYTE wIndex;
	virtual void PackToBuf();
	virtual BOOL LoadFromBuf();
	UINT tarIP;
	UINT nPort;
protected:
	int BP; //buffer pointer
};

inline void CopyDataBuf(CData* targe,CData*source)
{
	for(int i=0;i<source->num;i++)
		targe->szBuf[i]=source->szBuf[i];
	targe->num=source->num;
	targe->nPort=source->nPort;
	targe->tarIP=source->tarIP;
}
inline void StrNCopy(char *target,const char*src,int n)
{
	for(int i=0;i<n;i++)
		target[i]=src[i];
}
inline BOOL StrNSame(const char* s1,const char* s2,int n1,int n2)
{
	if(n1!=n2)return FALSE;
	for(int i=0;i<n1;i++)
		if(s1[i]!=s2[i])return FALSE;
	return TRUE;
}
//内联函数一定要在头文件里吗？
#endif // !defined(AFX_DATA_H__C0A9FB14_44DC_11D5_A7F9_00E04C41C3AD__INCLUDED_)
