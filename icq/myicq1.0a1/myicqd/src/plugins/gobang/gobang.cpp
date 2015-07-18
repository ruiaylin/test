#include "gobang.h"
#include "group.h"
#include "packet.h"
#include <memory.h>
#include <string.h>


#define ISVALID(row, col)	((unsigned) row < BOARD_SIZE && (unsigned) col < BOARD_SIZE)

enum {
	CMD_MOVE = 0x1000,
};


GobangGame::GobangGame(Group *g)
{
	group = g;
	isStarted = false;
	blackIndex = 0;
}

void GobangGame::onStart()
{
	isStarted = true;
	nextTurn = blackIndex;
	memset(board, 0, sizeof(board));
}

void GobangGame::destroy()
{
	delete this;
}

bool GobangGame::checkForWin(int row, int col)
{
	static int xoffset[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	static int yoffset[] = { 0, 1, 1, 1, 0, -1, -1, -1 };

	uint8 c = board[row][col];
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

void GobangGame::onMove(InPacket &in, int from)
{
	if (!isStarted || from != nextTurn)
		return;

	uint8 row, col;
	in >> row >> col;

	// Check if the move is illegal
	if (row >= BOARD_SIZE || col >= BOARD_SIZE || board[row][col])
		return;

	nextTurn ^= 1;
	board[row][col] = (blackIndex == from ? CELL_BLACK : CELL_WHITE);

	// Notify the other player...
	OutPacket *out = group->createPacket(CMD_MOVE);
	*out << row << col;
	group->sendPacket(out, nextTurn, from);

	if (checkForWin(row, col)) {
		isStarted = false;
		blackIndex ^= 1;
		group->restart();
	}
}

void GobangGame::onPacketReceived(InPacket &in, int from)
{
	uint16 cmd;
	in >> cmd;

	switch (cmd) {
	case CMD_MOVE:
		onMove(in, from);
		break;
	}
}


static GroupSession *createGroup(Group *g)
{
	return new GobangGame(g);
}


ICQ_EXPORT void getPluginInfo(PluginInfo *info)
{
	info->maxMembers = 2;
	info->createGroup = createGroup;
}
