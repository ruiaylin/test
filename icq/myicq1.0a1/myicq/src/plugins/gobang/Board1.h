#if !defined(AFX_BOARD_H__09CEC013_C4ED_41F9_8637_F580CBA452DD__INCLUDED_)
#define AFX_BOARD_H__09CEC013_C4ED_41F9_8637_F580CBA452DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Board.h : header file
//

#include "game.h"

/////////////////////////////////////////////////////////////////////////////
// Board window

class Board : public CStatic, public GobangGame
{
// Construction
public:
	Board(Group *g);

// Attributes
public:

// Operations
public:
	virtual void onGameFailed() {}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Board)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~Board();

private:
	void drawChessman(CDC *pDC, int row, int col, bool black);

	CImageList chessmanImageList;

	// Generated message map functions
protected:
	//{{AFX_MSG(Board)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOARD_H__09CEC013_C4ED_41F9_8637_F580CBA452DD__INCLUDED_)
