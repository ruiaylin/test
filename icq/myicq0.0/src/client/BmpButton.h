#if !defined(AFX_BMPBUTTON_H__F8626211_5ECA_11D5_B18A_9B8F4BBFF506__INCLUDED_)
#define AFX_BMPBUTTON_H__F8626211_5ECA_11D5_B18A_9B8F4BBFF506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BmpButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBmpButton window

class CBmpButton : public CButton
{
// Construction
public:
	CBmpButton();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL
	CBitmap m_bitmap;	

// Implementation
public:
	void SetBitmap(CBitmap*pBit);
	virtual ~CBmpButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBmpButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPBUTTON_H__F8626211_5ECA_11D5_B18A_9B8F4BBFF506__INCLUDED_)
