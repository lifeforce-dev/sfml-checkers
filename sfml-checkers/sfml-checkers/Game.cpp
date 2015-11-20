//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include <iostream>

Game::Game()
	: m_boardData(s_boardSize, std::vector<PieceDisplayType>(s_boardSize, EMPTY))
{
	Setup();
}

Game::~Game()
{
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

//-----------------------------------------------------------------------------

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
