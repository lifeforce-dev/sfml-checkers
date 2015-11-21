//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "CheckersTypes.h"

#include <vector>

typedef std::vector<std::vector<PieceDisplayType>> BoardData;

class Game
{
public:
	Game();
	~Game();

	const BoardData& GetBoardData() { return m_boardData; }
	std::pair<int, int> GetBoardIndexFromRowCol(int row, int col);

private:
	void Setup();
	void DEBUG_PRINT_BOARD();

	BoardData m_boardData;
};
