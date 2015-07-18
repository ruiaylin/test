/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2002 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

// RichEditCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "RichEditCtrlEx.h"
#include "ImageDataObject.h"
#include "icqclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
// Owner:   Andrew Forget (AndrewFo,x3438) 
//------------------------------------------------------------------------------
//
// @mfunc   | _AFX_RICHEDITEX_STATE | _AFX_RICHEDITEX_STATE |
//          Constructor for <c _AFX_RICHEDITEX_STATE>.
//
// @syntax  _AFX_RICHEDITEX_STATE() ;
//
// @version
//------------------------------------------------------------------------------
//.Version: Date:       Author:     Comments:
//.-------- ----------- ----------- --------------------------------------------
//.  1.0    01/18/99    AndrewFo    New code.
//******************************************************************************

_AFX_RICHEDITEX_STATE::_AFX_RICHEDITEX_STATE()
{
	m_hInstRichEdit20 = NULL ;
}

//******************************************************************************
// Owner:   Andrew Forget (AndrewFo,x3438) 
//------------------------------------------------------------------------------
//
// @mfunc   | _AFX_RICHEDITEX_STATE | ~_AFX_RICHEDITEX_STATE |
//          Destructor for <c _AFX_RICHEDITEX_STATE>.
//
// @syntax  ~_AFX_RICHEDITEX_STATE() ;
//
// @version
//------------------------------------------------------------------------------
//.Version: Date:       Author:     Comments:
//.-------- ----------- ----------- --------------------------------------------
//.  1.0    01/18/99    AndrewFo    New code.
//******************************************************************************

_AFX_RICHEDITEX_STATE::~_AFX_RICHEDITEX_STATE()
{
	if( m_hInstRichEdit20 != NULL )
	{
		::FreeLibrary( m_hInstRichEdit20 ) ;
	}
}


//******************************************************************************
//
// Global Data
//
//******************************************************************************

_AFX_RICHEDITEX_STATE _afxRichEditStateEx ;


//******************************************************************************
// Owner:   Andrew Forget (AndrewFo,x3438) 
//------------------------------------------------------------------------------
//
// @func    BOOL PASCAL | AfxInitRichEditEx |
//          This function must be called prior to creating any dialog which
//          uses the version 2.0 rich edit control.  For created classes, this
//          is called automatically.
//
// @syntax  BOOL PASCAL AfxInitRichEditEx() ;
//
// @rdesc   BOOL : A boolean value indicating:
// @flag        TRUE  | Success.
// @flag        FALSE | Failure.
//
// @comm    NOTE: This function will not work if AfxInitRichEdit() has been
//          called first and this will clash with the use of the CRichEditCtrl
//          class (including the CRichEditView class).
//
// @version
//------------------------------------------------------------------------------
//.Version: Date:       Author:     Comments:
//.-------- ----------- ----------- --------------------------------------------
//.  1.0    01/18/99    AndrewFo    New code.
//******************************************************************************

BOOL PASCAL AfxInitRichEditEx()
{
	if( ! ::AfxInitRichEdit() )
	{
		return FALSE ;
	}

	_AFX_RICHEDITEX_STATE* l_pState = &_afxRichEditStateEx ;

	if( l_pState->m_hInstRichEdit20 == NULL )
	{
		l_pState->m_hInstRichEdit20 = LoadLibraryA( "RICHED20.DLL" ) ;
	}

	return l_pState->m_hInstRichEdit20 != NULL ;
}



LPCTSTR CRichEditCtrlEx::emotions[] = {
	"/:O", "/:-/", "/:*", "/>:", "/8-)", "/:(", "/:$", "/:-X", "/:-Z", "/:'(",
	"/:-|", "/:@", "/:P", "/:D", "/:)", "/<D>", "/<J>", "/<H>", "/<M>", "/<QQ>",
	"/<MM>", "/<L>", "/<S>", "/<K>", "/<T>", "/<$>", "/<O>", "/<&>", "/<B>", "/<F>",
	"/<U>", "/<V>", "/<W>", "/<Y1>", "/<Y2>", "/<%>", "/<@@>", "/<X>", "/<I>", "/<G>",
	"/<!!>", "/<~>", "/<C>", "/<Z>", "/<*>", "/<)>", "/<OK>", "/<NO>", "/<00>", "/<11>",
};

int CRichEditCtrlEx::findEmotion(const char *str)
{
	for (int i = 0; i < NR_EMOTIONS; ++i) {
		const char *dst = emotions[i];
		const char *src = str;
		while (*++dst && *++src && *dst == *src);

		if (!*dst)
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx

CRichEditCtrlEx::CRichEditCtrlEx()
{
}

CRichEditCtrlEx::~CRichEditCtrlEx()
{
}

HBITMAP CRichEditCtrlEx::GetImage(CImageList& il, int num)
{
	CBitmap dist;
	CClientDC dc(NULL);

	IMAGEINFO ii;
	il.GetImageInfo(num, &ii);

	int nWidth = ii.rcImage.right - ii.rcImage.left;
	int nHeight = ii.rcImage.bottom - ii.rcImage.top;

	dist.CreateCompatibleBitmap(&dc, nWidth, nHeight);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap* pOldBitmap = memDC.SelectObject(&dist);
	
	COLORREF oldColor = SetBackgroundColor(TRUE, 0);
	memDC.FillSolidRect(0, 0, nWidth, nHeight, oldColor);
	SetBackgroundColor(FALSE, oldColor);
	il.Draw(&memDC, num, CPoint(0, 0), ILD_NORMAL);

	memDC.SelectObject(pOldBitmap);

	return (HBITMAP)dist.Detach();
}

void CRichEditCtrlEx::insertEmotion(int i)
{
	CImageDataObject::InsertBitmap(GetIRichEditOle(),
		GetImage(getApp()->emotionImageList, i), i);
}

void CRichEditCtrlEx::insertText(LPCTSTR text)
{
	LPCTSTR p, start, prev;
	start = prev = text;

	while ((p = strchr(prev, '/')) != NULL) {
		int i = findEmotion(p);
		if (i >= 0) {
			ReplaceSel(CString(start, p - start));
			insertEmotion(i);
			start = prev = p + strlen(emotions[i]);
		} else
			prev = p + 1;
	}

	ReplaceSel(start);
}

CString CRichEditCtrlEx::getText(CString &text)
{	
	CString str;
	GetWindowText(str);

	IRichEditOle *ole = GetIRichEditOle();
	int n = ole->GetObjectCount();
	REOBJECT reo;
	reo.cbStruct = sizeof(reo);

	text.Empty();
	int pos = 0;
	for (int i = 0; i < n; ++i) {
		ole->GetObject(i, &reo, REO_GETOBJ_NO_INTERFACES);
		text += str.Mid(pos, reo.cp - pos);
		text += emotions[reo.dwUser];
		pos = reo.cp + 1;
	}
	text += str.Mid(pos);

	return text;
}

void CRichEditCtrlEx::setText(LPCTSTR text)
{
	SetSel(0, -1);
	Clear();
	insertText(text);
}

void CRichEditCtrlEx::insertMsg(IcqMsg &msg)
{
	BOOL b = (GetStyle() & ES_READONLY);
	SetReadOnly(FALSE);

	CHARRANGE cr;
	GetSel(cr);
	insertText(msg.text.c_str());

	MsgFormat &fmt = msg.format;
	
	if (fmt.fontSize && !fmt.fontName.empty()) {
		CHARFORMAT cf;
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_FACE | CFM_SIZE | CFM_COLOR;

		strncpy(cf.szFaceName, fmt.fontName.c_str(), LF_FACESIZE);
		cf.yHeight = fmt.fontSize * 20;
		cf.crTextColor = fmt.fontColor;

		uint8 flags = fmt.flags;
		cf.dwEffects = 0;
		if (flags & MFF_BOLD)
			cf.dwEffects |= CFE_BOLD;
		if (flags & MFF_ITALIC)
			cf.dwEffects |= CFE_ITALIC;
		if (flags & MFF_UNDERLINE)
			cf.dwEffects |= CFE_UNDERLINE;

		SetSel(cr.cpMin, -1);
		SetSelectionCharFormat(cf);
	}

	SetSel(-1, -1);
	SetReadOnly(b);
}

void CRichEditCtrlEx::scrollToBottom()
{
	// get the current end of the window
	CPoint point = GetCharPos(GetTextLength());
	CRect rect;
	GetClientRect(&rect);

	// scroll the bottom into view
	while (point.y > rect.bottom) {
		LineScroll(1);
		point = GetCharPos(GetTextLength());
	}
}


BEGIN_MESSAGE_MAP(CRichEditCtrlEx, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEditCtrlEx)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(EN_LINK, OnLink)
END_MESSAGE_MAP()

//******************************************************************************
// Owner:   Andrew Forget (AndrewFo,x3438) 
//------------------------------------------------------------------------------
//
// @mfunc   BOOL | CRichEditCtrlEx | Create |
//          This method is used to create the version 2.0 rich edit control.
//
// @syntax  BOOL Create(
//          			DWORD		in_dwStyle, 
//          			const RECT& in_rcRect, 
//          			CWnd*		in_pParentWnd, 
//          			UINT		in_nID ) ;
//
// @parm    DWORD | in_dwStyle |
//          The styles for the rich edit control.
//
// @parm    const RECT& | in_rcRect |
//          The rectangle for the control window.
//
// @parm    CWnd* | in_pParentWnd |
//          Pointer to the parent window.
//
// @parm    UINT | in_nID |
//          The dialog control ID for the control.
//
// @rdesc   BOOL : A boolean value indicating:
// @flag        TRUE  | Sucess.
// @flag        FALSE | Failure.
//
// @version
//------------------------------------------------------------------------------
//.Version: Date:       Author:     Comments:
//.-------- ----------- ----------- --------------------------------------------
//.  1.0    01/18/99    AndrewFo    New code.
//******************************************************************************

BOOL CRichEditCtrlEx::Create(
			DWORD		in_dwStyle, 
			const RECT& in_rcRect, 
			CWnd*		in_pParentWnd, 
			UINT		in_nID )
{
	if( ! ::AfxInitRichEditEx() )
	{
		return FALSE ;
	}

	CWnd* l_pWnd = this ;

	return l_pWnd->Create( _T( "RichEdit20A" ), NULL, in_dwStyle, in_rcRect, in_pParentWnd, in_nID ) ;
}

//******************************************************************************
// Owner:   Andrew Forget (AndrewFo,x3438) 
//------------------------------------------------------------------------------
//
// @mfunc   BOOL | CRichEditCtrlEx | AutoURLDetect |
//          This method sets the URL auto detection to enabled or disabled.
//
// @syntax  inline BOOL AutoURLDetect( 
//						BOOL in_fEnable ) ;
//
// @parm    BOOL | in_fEnable |
//          TRUE to enable URL auto detection.
//
// @rdesc   BOOL : A boolean value indicating:
// @flag        TRUE  | Success.
// @flag        FALSE | Failure.
//
// @version
//------------------------------------------------------------------------------
//.Version: Date:       Author:     Comments:
//.-------- ----------- ----------- --------------------------------------------
//.  1.0    01/18/99    AndrewFo    New code.
//******************************************************************************

BOOL CRichEditCtrlEx::AutoURLDetect( BOOL in_fEnable )
{
	ASSERT(::IsWindow( m_hWnd ) ) ;
	if (::SendMessage( m_hWnd, EM_AUTOURLDETECT, in_fEnable, 0 ))
		return FALSE;

	// @@
	if (in_fEnable)
		SetEventMask(GetEventMask() | ENM_LINK);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx message handlers

void CRichEditCtrlEx::OnLink(NMHDR *pNMHDR, LRESULT *result)
{
	ENLINK *link = (ENLINK *) pNMHDR;

	*result = 0;

	if (link->msg == WM_LBUTTONDOWN) {
		CHARRANGE cr;
		GetSel(cr);
		SetSel(link->chrg);
		CString url = GetSelText();
		SetSel(cr);

		ShellExecute(NULL, _T("open"), url, NULL,NULL, SW_SHOW);

		*result = 1;
	}
}
