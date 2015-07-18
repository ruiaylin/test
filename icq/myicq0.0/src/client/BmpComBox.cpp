// BmpComBox.cpp : implementation file
//

#include "stdafx.h"
#include "BmpComBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpComBox

CBmpComBox::CBmpComBox()
{
	m_size=CSize(32,32);
	m_pimaList=NULL;
}

CBmpComBox::~CBmpComBox()
{
}


BEGIN_MESSAGE_MAP(CBmpComBox, CComboBox)
	//{{AFX_MSG_MAP(CBmpComBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpComBox message handlers

void CBmpComBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (!IsWindowEnabled())
	{
        CBrush brDisabled(RGB(192,192,192)); // light gray
		CBrush* pOldBrush = pDC->SelectObject(&brDisabled);
		CPen penDisabled(PS_SOLID, 1, RGB(192,192,192));
        CPen* pOldPen = pDC->SelectObject(&penDisabled);
        OnOutputBmp(lpDrawItemStruct, FALSE);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		return;
	}

	// Selected
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) 
		&& (lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) 
	{
		CBrush brHighlight(::GetSysColor(COLOR_HIGHLIGHT)); 
		CBrush* pOldBrush = pDC->SelectObject(&brHighlight);
		CPen penHighlight(PS_SOLID, 1, ::GetSysColor(COLOR_HIGHLIGHT));
		CPen* pOldPen = pDC->SelectObject(&penHighlight);
		pDC->Rectangle(&lpDrawItemStruct->rcItem);
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
       	pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		OnOutputBmp(lpDrawItemStruct, TRUE);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	// De-Selected
	if (!(lpDrawItemStruct->itemState & ODS_SELECTED) 
		&& (lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) 
	{
		CBrush brWindow(::GetSysColor(COLOR_WINDOW)); 
		CBrush* pOldBrush = pDC->SelectObject(&brWindow);
		CPen penHighlight(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOW));
		CPen* pOldPen = pDC->SelectObject(&penHighlight);
		pDC->Rectangle(&lpDrawItemStruct->rcItem);
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
       	pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		OnOutputBmp(lpDrawItemStruct, FALSE);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

    // Focus
    if (lpDrawItemStruct->itemAction & ODA_FOCUS) 
	{ 
        pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
    }	
}

void CBmpComBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = (m_size.cy + 2); 	
}

void CBmpComBox::OnOutputBmp(LPDRAWITEMSTRUCT lpDrawItemStruct, BOOL bSelected)
{
	if (GetCurSel() == CB_ERR || GetCount() == 0) 
		return; // no item selected
    
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	int index=lpDrawItemStruct->itemData;

	int x = lpDrawItemStruct->rcItem.left + ((lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left) / 2) - (m_size.cx / 2);
	int y = lpDrawItemStruct->rcItem.top + ((lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top) / 2) - (m_size.cy / 2);
	
	m_pimaList->Draw(pDC,index,CPoint(x,y),ILD_NORMAL);
}

void CBmpComBox::SetImageList(CImageList *pList)
{
	ASSERT(pList!=NULL);
	m_pimaList=pList;
	int i,index;
	for(i=0;i<pList->GetImageCount();i++)
	{
		index=AddString("");
		SetItemData(index,index);
	}
	IMAGEINFO iInfo;
	pList->GetImageInfo(0,&iInfo);
	m_size=CRect(iInfo.rcImage).Size();
	SetItemHeight(-1,m_size.cy);
}

void CBmpComBox::SetItemSize(CSize size)
{
	m_size=size;
	SetItemHeight(-1,m_size.cy);
}
