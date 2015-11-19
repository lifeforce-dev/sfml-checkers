//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "CheckersTypes.h"

#include <vector>

typedef std::vector<std::vector<SpaceStatus>> BoardData;

class Game
{
public:
	Game();
	~Game();

private:
	void Setup();
	void DEBUG_PRINT_BOARD();

	BoardData m_boardData;
};