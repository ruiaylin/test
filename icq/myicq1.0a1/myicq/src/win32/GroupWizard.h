#if !defined(AFX_GROUPWIZARD_H__46BAD352_3108_4F6D_915B_CC6BF4060530__INCLUDED_)
#define AFX_GROUPWIZARD_H__46BAD352_3108_4F6D_915B_CC6BF4060530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupWizard.h : header file
//

#include "icqwindow.h"
#include "GroupModePage.h"
#include "GroupListPage.h"
#include "GroupFinishPage.h"

enum {
	GROUP_MODE_CREATE,
	GROUP_MODE_LIST,
	GROUP_MODE_SEARCH,
};

/////////////////////////////////////////////////////////////////////////////
// CGroupWizard

class CGroupWizard : public CPropertySheet, public IcqWindow
{
	DECLARE_DYNAMIC(CGroupWizard)

// Construction
public:
	CGroupWizard();

	void onGameListReply(GAME_INFO info[], int n) {
		if (GetActivePage() == &modePage)
			modePage.onGameListReply(info, n);
	}
	void onCreateGroupReply(uint32 id) {
		if (GetActivePage() == &finishPage)
			finishPage.onCreateGroupReply(id);
	}
	void onGroupListReply(GROUP_INFO info[], int n) {
		if (GetActivePage() == &listPage)
			listPage.onGroupListReply(info, n);
	}

	int gameType;
	CString gamePlugin;

	CGroupModePage modePage;
	CGroupListPage listPage;
	CGroupFinishPage finishPage;

// Operations
public:
	BOOL Create(CWnd *parent = NULL);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupWizard)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGroupWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGroupWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPWIZARD_H__46BAD352_3108_4F6D_915B_CC6BF4060530__INCLUDED_)
