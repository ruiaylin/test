#ifndef _GOBANG_H
#define _GOBANG_H

#include "groupsession.h"
#include "icqtypes.h"

#define BOARD_SIZE		15

enum {
	CELL_NONE,
	CELL_BLACK,
	CELL_WHITE,
};

class Group;

class GobangGame : public GroupSession {
public:
	GobangGame(Group *g);

	virtual void onMemberEntered(int i) {}
	virtual void onMemberExited(int i) {}
	virtual void onStart();

	virtual void destroy();
	virtual void onPacketReceived(InPacket &in, int from);

private:
	bool checkForWin(int row, int col);

	void onMove(InPacket &in, int from);

	uint8 board[BOARD_SIZE][BOARD_SIZE];
	Group *group;
	int blackIndex;
	int nextTurn;
	bool isStarted;
};


#endif
