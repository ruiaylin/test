#if !defined(AFX_GROUPMODEPAGE_H__555C9E36_9F60_4F30_A443_82A3DFE1168C__INCLUDED_)
#define AFX_GROUPMODEPAGE_H__555C9E36_9F60_4F30_A443_82A3DFE1168C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupModePage.h : header file
//

struct GAME_INFO;

/////////////////////////////////////////////////////////////////////////////
// CGroupModePage dialog

class CGroupModePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupModePage)

// Construction
public:
	CGroupModePage();
	~CGroupModePage();

	void onGameListReply(GAME_INFO info[], int n);

// Dialog Data
	//{{AFX_DATA(CGroupModePage)
	enum { IDD = IDD_GROUP_MODE };
	CListCtrl	m_gameList;
	int		m_mode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupModePage)
	public:
	virtual void OnCancel();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	int getSelectedItem();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupModePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPMODEPAGE_H__555C9E36_9F60_4F30_A443_82A3DFE1168C__INCLUDED_)
