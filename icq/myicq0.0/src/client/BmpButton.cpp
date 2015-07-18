// BmpButton.cpp : implementation file
//

#include "stdafx.h"
#include "BmpButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpButton

CBmpButton::CBmpButton()
{	
}

CBmpButton::~CBmpButton()
{
}


BEGIN_MESSAGE_MAP(CBmpButton, CButton)
	//{{AFX_MSG_MAP(CBmpButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpButton message handlers

void CBmpButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(m_bitmap.GetObject==NULL)
		return;

	UINT    nOffset=0; 
	UINT	nFrameStyle=0;
	BOOL	bDRAWFOCUSONLY = FALSE;
	int nState=lpDrawItemStruct->itemState;		
	UINT	nNewAction = lpDrawItemStruct->itemAction;
	
	if ( nState & ODS_SELECTED)
	{
		nFrameStyle = DFCS_PUSHED;
		nOffset += 1;
	}
	nState=DSS_NORMAL;
	if (nNewAction == ODA_FOCUS )
		bDRAWFOCUSONLY = TRUE;
	
	CRect rt;
	GetClientRect(&rt);
	if(!bDRAWFOCUSONLY)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CRect rtControl( lpDrawItemStruct->rcItem );
		CPoint pt(rtControl.left,rtControl.top);
		CSize sizeDraw;		
		sizeDraw.cx=rt.Width();
		sizeDraw.cy=rt.Height();			
		pt.Offset(nOffset,nOffset);
		pDC->Rectangle( &lpDrawItemStruct->rcItem );		
		pDC->DrawFrameControl(&rtControl, DFC_BUTTON, DFCS_BUTTONPUSH | nFrameStyle);	
		pDC->DrawState(pt,sizeDraw,m_bitmap,DST_BITMAP|nState);	
	}
}

void CBmpButton::SetBitmap(CBitmap *pBit)
{	
	CRect rt;
	GetClientRect(&rt);
	m_bitmap.DeleteObject();		
	HBITMAP hb=(HBITMAP)::CopyImage(pBit->GetSafeHandle(),IMAGE_BITMAP,rt.Width(),rt.Height(),LR_CREATEDIBSECTION);
	m_bitmap.Attach(hb);
	Invalidate();
}
