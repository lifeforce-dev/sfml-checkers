//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"
#include "Log.h"

#include <assert.h>

Game::Game()
	: m_boardData(s_boardSize, std::vector<PieceDisplayType>(s_boardSize, EMPTY))
	, m_pendingMoveLauncher(new CheckersMoveLauncher(std::bind(&Game::OnMoveSelectionFinalized, this)))
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

void Game::OnMoveSelectionEvent(const BoardIndex& boardIndex)
{
	assert(IsValidBoardIndex(boardIndex));

	m_pendingMoveLauncher->HandleMoveSelected(boardIndex);
}

void Game::OnMoveSelectionFinalized()
{
	m_pendingMoveLauncher.reset(new CheckersMoveLauncher(std::bind(&Game::OnMoveSelectionFinalized, this)));
}

void Game::Setup()
{
	for (int row = 0; row < s_boardSize; ++row)
	{
		for (int col = 0; col < s_boardSize; ++col)
		{
			// Find the correct diagonal.
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

CheckersMoveLauncher::CheckersMoveLauncher(std::function<void()> launchMoveCallback)
	: m_moveSource(-1, -1)
	, m_moveDestination(-1, -1)
	, m_isSourceSet(false)
	, m_isDestinationSet(false)
	, m_launchMoveCallback(launchMoveCallback)
{
}

CheckersMoveLauncher::~CheckersMoveLauncher()
{
}

void CheckersMoveLauncher::HandleMoveSelected(const BoardIndex& move)
{
	if (m_isDestinationSet)
	{
		assert(m_isSourceSet);

		LOG_DEBUG_CONSOLE("Warning: This move already has a source and a destination.");
		return;
	}

	// The first selected move is the source.
	if (!m_isSourceSet)
	{
		m_moveSource = move;
		m_isSourceSet = true;
	}
	else
	{
		// Next is the destination.
		m_moveDestination = move;
		m_isDestinationSet = true;

		// Ready to move, launch!
		m_launchMoveCallback();
	}
}

const BoardIndex& CheckersMoveLauncher::GetMoveSource() const
{
	return m_moveSource;
}

const BoardIndex& CheckersMoveLauncher::GetMoveDestination() const
{
	return m_moveDestination;
}
