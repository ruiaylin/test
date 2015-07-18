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

#include "stdafx.h"
#include "game.h"
#include "group.h"
#include "packet.h"

#define ISVALID(row, col)	\
	(row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE)

enum {
	CMD_MOVE = 0x1000,
};


GobangGame::GobangGame(Group *g)
{
	group = g;
	icqLink = g->getLink();

	isStarted = false;
	myPos = g->getMyPos();
	isBlack = (myPos == 0);

	init();
}

void GobangGame::init()
{
	memset(board, 0, sizeof(board));
}

void GobangGame::onStart()
{
	isStarted = true;
	nextTurn = myPos;
	if (!isBlack)
		nextTurn ^= 1;

	init();
}

void GobangGame::start()
{
	group->start();
	init();
}

bool GobangGame::checkForWin(int row, int col)
{
	static int xoffset[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	static int yoffset[] = { 0, 1, 1, 1, 0, -1, -1, -1 };

	int c = board[row][col];
	int sum[4];

	memset(sum, 0, sizeof(sum));

	for (int k = 0; k < 8; k++) {
		int i = row + yoffset[k];
		int j = col + xoffset[k];

		while (ISVALID(i, j) && board[i][j] == c) {
			if (++sum[k & 3] >= 4)
				return true;

			i += yoffset[k];
			j += xoffset[k];
		}
	}
	return false;
}

int GobangGame::move(int row, int col)
{
	if (!isStarted || nextTurn != myPos || !ISVALID(row, col) || board[row][col])
		return MOVE_FAILED;

	nextTurn ^= 1;

	OutPacket *out = group->createPacket(CMD_MOVE);
	*out << (uint8) row << (uint8) col;
	group->sendPacket(out);

	board[row][col] = (isBlack ? CELL_BLACK : CELL_WHITE);

	if (checkForWin(row, col)) {
		isStarted = false;
		isBlack = !isBlack;
		return MOVE_WIN;
	}

	return MOVE_SUCCESS;
}

void GobangGame::onMove(InPacket &in)
{
	if (nextTurn == myPos)
		return;

	uint8 row, col;
	in >> row >> col;

	// Check if the move is illegal
	if (!ISVALID(row, col) || board[row][col])
		return;

	board[row][col] = (isBlack ? CELL_WHITE : CELL_BLACK);
	nextTurn ^= 1;

	int res = MOVE_SUCCESS;

	if (checkForWin(row, col)) {
		res = MOVE_LOSE;
		isBlack = !isBlack;
		isStarted = false;
	}
	onMove(row, col, res);
}

void GobangGame::onPacketReceived(InPacket &in)
{
	uint16 cmd;
	in >> cmd;

	switch (cmd) {
	case CMD_MOVE:
		onMove(in);
		break;
	}
}
