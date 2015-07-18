// Board.cpp : implementation file
//

#include "stdafx.h"
#include "gobang.h"
#include "Board.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OFFSET_X	18
#define OFFSET_Y	18
#define CELL_SIZE	24


/////////////////////////////////////////////////////////////////////////////
// Board

Board::Board(Group *g) : GobangGame(g)
{
	chessmanImageList.Create(20, 20, ILC_COLOR8 | ILC_MASK, 2, 0);

	CBitmap bm;
	bm.LoadBitmap(IDB_CHESSMAN);
	chessmanImageList.Add(&bm, RGB(0, 128, 128));
}

Board::~Board()
{
}

void Board::drawChessman(CDC *pDC, int row, int col, bool black)
{
	POINT pt;
	pt.x = OFFSET_X + col * CELL_SIZE;
	pt.y = OFFSET_Y + row * CELL_SIZE;
	int index = (black ? 0 : 1);
	chessmanImageList.Draw(pDC, index, pt, ILD_TRANSPARENT);
}

BEGIN_MESSAGE_MAP(Board, CStatic)
	//{{AFX_MSG_MAP(Board)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Board message handlers

void Board::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CBitmap bm;
	bm.Attach(GetBitmap());
	BITMAP info;
	bm.GetBitmap(&info);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap *oldBitmap = memDC.SelectObject(&bm);

	dc.BitBlt(0, 0, info.bmWidth, info.bmHeight, &memDC, 0, 0, SRCCOPY);

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == CELL_NONE)
				continue;

			drawChessman(&dc, i, j, board[i][j] == CELL_BLACK);
		}
	}

	memDC.SelectObject(oldBitmap);
	bm.Detach();
}

void Board::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int row = ((point.y - OFFSET_Y) + CELL_SIZE / 2) / CELL_SIZE;
	int col = ((point.x - OFFSET_X) + CELL_SIZE / 2) / CELL_SIZE;

	if (move(row, col)) {
		CClientDC dc(this);
		drawChessman(&dc, row, col, isBlack);
	}

	CStatic::OnLButtonDown(nFlags, point);
}
