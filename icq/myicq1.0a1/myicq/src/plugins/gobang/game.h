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

#ifndef _GAME_H
#define _GAME_H

#include "groupsession.h"

#define BOARD_SIZE		15

enum {
	CELL_NONE,
	CELL_BLACK,
	CELL_WHITE,
};

enum {
	MOVE_FAILED,
	MOVE_SUCCESS,
	MOVE_WIN,
	MOVE_LOSE,
};

class Group;
class IcqLinkBase;

class GobangGame : public GroupSession {
public:
	GobangGame(Group *g);

	virtual void onMemberEntered(int i) {};
	virtual void onStart();

	virtual void onPacketReceived(InPacket &in);

	int move(int row, int col);

	void onMove(InPacket &in);

protected:
	virtual void onMove(int row, int col, int res) = 0;

	void init();
	void start();
	bool checkForWin(int row, int col);

	Group *group;
	IcqLinkBase *icqLink;

	int board[BOARD_SIZE][BOARD_SIZE];
	int nextTurn;
	int myPos;
	bool isBlack;
	bool isStarted;
};


#endif
