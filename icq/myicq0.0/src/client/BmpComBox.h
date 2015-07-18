#if !defined(AFX_BMPCOMBOX_H__4996F956_60CE_11D5_B19A_86C046F2DB04__INCLUDED_)
#define AFX_BMPCOMBOX_H__4996F956_60CE_11D5_B19A_86C046F2DB04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BmpComBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBmpComBox window

class CBmpComBox : public CComboBox
{
// Construction
public:
	CBmpComBox();

// Attributes
private:
	CSize m_size;
	CImageList * m_pimaList;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpComBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetItemSize(CSize size);
	void SetImageList(CImageList* pList);
	virtual ~CBmpComBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBmpComBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void OnOutputBmp(LPDRAWITEMSTRUCT lpDrawItemStruct, BOOL bSelected);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPCOMBOX_H__4996F956_60CE_11D5_B19A_86C046F2DB04__INCLUDED_)
