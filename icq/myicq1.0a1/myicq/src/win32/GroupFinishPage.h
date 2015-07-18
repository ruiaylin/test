#if !defined(AFX_GROUPFINISHPAGE_H__C9F6FBC4_4297_4DB2_8790_73FDBDFE0BFB__INCLUDED_)
#define AFX_GROUPFINISHPAGE_H__C9F6FBC4_4297_4DB2_8790_73FDBDFE0BFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupFinishPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupFinishPage dialog

class CGroupFinishPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupFinishPage)

// Construction
public:
	CGroupFinishPage();
	~CGroupFinishPage();

	void onCreateGroupReply(uint32 id);

// Dialog Data
	//{{AFX_DATA(CGroupFinishPage)
	enum { IDD = IDD_GROUP_FINISH };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupFinishPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupFinishPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPFINISHPAGE_H__C9F6FBC4_4297_4DB2_8790_73FDBDFE0BFB__INCLUDED_)
