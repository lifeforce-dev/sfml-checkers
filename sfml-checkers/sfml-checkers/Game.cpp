//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"
#include "Log.h"

Game::Game()
	: m_boardData(s_boardSize, std::vector<PieceDisplayType>(s_boardSize, EMPTY))
{
	Setup();
}

Game::~Game()
{
}

BoardIndex Game::GetBoardIndexFromRowCol(int row, int col) const
{
	if (!IsValidBoardIndex(std::make_pair(row, col)))
		return BoardIndex();

	return std::make_pair(row, col);
}

void Game::OnMoveEvent(const BoardIndex& boardIndex)
{
	if (!IsValidBoardIndex(boardIndex))
		return;
}

void Game::Setup()
{
	for (int row = 0; row < s_boardSize; ++row)
	{
		for (int col = 0; col < s_boardSize; ++col)
		{
			// Find the right diagonal
			if ((row + col) % 2)
			{
				if (row > 4)
					m_boardData[row][col] = BLACK;
				else if (row < 3)
					m_boardData[row][col] = WHITE;
				else
					m_boardData[row][col] = EMPTY;
			}
		}
	}
}

bool Game::IsValidBoardIndex(const BoardIndex& boardIndex) const
{
	int row = boardIndex.first;
	int col = boardIndex.second;

	bool isValid = row >= 0 || row < s_boardSize || col >= 0 || col < s_boardSize;

	if (!isValid)
	{
		LOG_DEBUG_CONSOLE("Error: attempt to access invalid index" + std::to_string(row)
			+ " , " + std::to_string(col));
	}

	return isValid;
}

void Game::DEBUG_PRINT_BOARD()
{
	for (auto it = m_boardData.begin(); it != m_boardData.end(); ++it)
	{
		for (auto ij = it->begin(); ij != it->end(); ++ij)
		{
			char symbol = ' ';
			switch (*ij)
			{
			case BLACK:
				symbol = 'X';
				break;
			case WHITE:
				symbol = 'O';
				break;
			case BLACK_KING:
				symbol = '*';
				break;
			case WHITE_KING:
				symbol = '&';
				break;
			case EMPTY:
				symbol = '.';
				break;
			}
			std::cout << symbol;
		}
		std::cout << "\n";
	}
}

//---------------------------------------------------------------

CheckersMove::CheckersMove()
{

}

CheckersMove::~CheckersMove()
{

}
