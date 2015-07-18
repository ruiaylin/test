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

// GobangDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gobang.h"
#include "GobangDlg.h"
#include "group.h"
#include "icqlinkbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BOARD_X			5
#define BOARD_Y			5
#define OFFSET_X		(BOARD_X + 18)
#define OFFSET_Y		(BOARD_Y + 18)
#define CELL_SIZE		24
#define CHESSMAN_SIZE	20

/////////////////////////////////////////////////////////////////////////////
// CGobangDlg dialog


CGobangDlg::CGobangDlg(Group *g, CWnd* pParent /*=NULL*/)
	: CDialog(CGobangDlg::IDD, pParent), GobangGame(g)
{
	//{{AFX_DATA_INIT(CGobangDlg)
	//}}AFX_DATA_INIT

	lastRow = lastCol = -1;

	boardBitmap.LoadBitmap(IDB_BOARD);
	chessmanImageList.Create(CHESSMAN_SIZE, CHESSMAN_SIZE, ILC_COLOR8 | ILC_MASK, 2, 0);
	
	CBitmap bm;
	bm.LoadBitmap(IDB_CHESSMAN);
	chessmanImageList.Add(&bm, RGB(0, 128, 128));

	for (int i = 0; i < 2; i++)
		chessmanIcon[i] = chessmanImageList.ExtractIcon(i);

	Create(IDD_GOBANG, GetDesktopWindow());
}

CGobangDlg::~CGobangDlg()
{
	for (int i = 0; i < 2; i++)
		DestroyIcon(chessmanIcon[i]);
}

void CGobangDlg::destroy()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DestroyWindow();
}

void CGobangDlg::onStart()
{
	GobangGame::onStart();

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	setStatus(isBlack ? IDS_STATUS_YOUR_TURN : IDS_STATUS_WAIT_TURN);
}

void CGobangDlg::onMemberExited(int i)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CString str;
	str.LoadString(IDS_PROMPT_CLOSED);
	MessageBox(str);
}

void CGobangDlg::onMove(int row, int col, int res)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CClientDC dc(this);

	if (lastRow >= 0 && lastCol >= 0)
		drawChessman(&dc, lastRow, lastCol);
	drawChessman(&dc, row, col);

	lastRow = row;
	lastCol = col;
	drawMark(&dc);

	setStatus(IDS_STATUS_YOUR_TURN);

	if (res == MOVE_LOSE) {
		CString str;
		str.LoadString(IDS_YOU_LOSE);
		MessageBox(str);

		setStatus(IDS_STATUS_START_GAME);
	}
}

void CGobangDlg::drawChessman(CDC *pDC, int row, int col)
{
	int c = board[row][col];
	if (c == CELL_NONE)
		return;
	
	POINT pt;
	pt.x = OFFSET_X + col * CELL_SIZE - CHESSMAN_SIZE / 2;
	pt.y = OFFSET_Y + row * CELL_SIZE - CHESSMAN_SIZE / 2;

	int i = (board[row][col] == CELL_BLACK ? 0 : 1);
	chessmanImageList.Draw(pDC, i, pt, ILD_TRANSPARENT);
}

void CGobangDlg::drawMark(CDC *pDC)
{
	if (lastRow < 0 || lastCol < 0)
		return;

	int x = OFFSET_X + lastCol * CELL_SIZE;
	int y = OFFSET_Y + lastRow * CELL_SIZE;

	CPen pen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen *oldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(x - 3, y);
	pDC->LineTo(x + 3, y);
	pDC->MoveTo(x, y - 3);
	pDC->LineTo(x, y + 3);

	pDC->SelectObject(oldPen);
}

void CGobangDlg::setStatus(UINT id)
{
	CString str;
	str.LoadString(id);
	SetDlgItemText(IDC_STATUS, str);
}

void CGobangDlg::updateInfo()
{
	int i = (isBlack ? 0 : 1);

	m_chessman[myPos].SetIcon(chessmanIcon[i]);
	m_chessman[myPos ^ 1].SetIcon(chessmanIcon[i ^ 1]);
}

void CGobangDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGobangDlg)
	DDX_Control(pDX, IDC_CHESSMAN1, m_chessman[0]);
	DDX_Control(pDX, IDC_CHESSMAN2, m_chessman[1]);
	DDX_Control(pDX, IDC_FACE1, m_faceLabel[0]);
	DDX_Control(pDX, IDC_FACE2, m_faceLabel[1]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGobangDlg, CDialog)
	//{{AFX_MSG_MAP(CGobangDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_START, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGobangDlg message handlers

void CGobangDlg::PostNcDestroy() 
{
	delete this;
}

void CGobangDlg::OnCancel()
{
	CString str;
	str.LoadString(IDS_PROMPT_EXIT);
	if (MessageBox(str, NULL, MB_YESNO | MB_ICONWARNING) != IDYES)
		return;

	group->destroy();
	DestroyWindow();
}

void CGobangDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	BITMAP info;
	boardBitmap.GetBitmap(&info);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap *oldBitmap = memDC.SelectObject(&boardBitmap);

	dc.BitBlt(BOARD_X, BOARD_Y, info.bmWidth, info.bmHeight, &memDC, 0, 0, SRCCOPY);

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == CELL_NONE)
				continue;

			drawChessman(&dc, i, j);
		}
	}
	drawMark(&dc);

	memDC.SelectObject(oldBitmap);
}

void CGobangDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int row = ((point.y - OFFSET_Y) + CELL_SIZE / 2) / CELL_SIZE;
	int col = ((point.x - OFFSET_X) + CELL_SIZE / 2) / CELL_SIZE;

	int res = move(row, col);
	if (res == MOVE_FAILED)
		return;

	CClientDC dc(this);
	if (lastRow >= 0 && lastCol >= 0)
		drawChessman(&dc, lastRow, lastCol);
	drawChessman(&dc, row, col);

	setStatus(IDS_STATUS_WAIT_TURN);

	if (res == MOVE_WIN) {
		CString str;
		str.LoadString(IDS_YOU_WIN);
		MessageBox(str);

		setStatus(IDS_STATUS_START_GAME);
	}

	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CGobangDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HICON icon = AfxGetApp()->LoadIcon(IDI_GOBANG);
	SetIcon(icon, FALSE);

	UINT ids[] = { IDC_NICK1, IDC_NICK2 };

	for (int i = 0; i < 2; i++) {
		GroupMember *m = group->getMemberInfo(i);
		icon = (HICON) icqLink->getFaceIcon(m->face, 0);
		m_faceLabel[i].SetIcon(icon);
		SetDlgItemText(ids[i], m->nick.c_str());
	}

	updateInfo();

	setStatus(IDS_STATUS_START_GAME);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGobangDlg::OnStart() 
{
	if (isStarted)
		return;
	
	setStatus(IDS_STATUS_WAIT_START);

	lastRow = lastCol = -1;
	updateInfo();
	Invalidate(FALSE);

	start();
}
