//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "CheckersTypes.h"

struct CheckersMove
{
	CheckersMove();
	~CheckersMove();

	BoardIndex m_moveStart;
	BoardIndex m_moveFinish;
};

class Game
{
public:
	Game();
	~Game();

	const BoardData& GetBoardData() { return m_boardData; }
	BoardIndex GetBoardIndexFromRowCol(int row, int col) const;

	void OnMoveEvent(const BoardIndex& boardIndex);

private:
	void Setup();
	bool IsValidBoardIndex(const BoardIndex& boardIndex) const;

	void DEBUG_PRINT_BOARD();

	BoardData m_boardData;
	CheckersMove m_pendingMove;
};
