//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"
#include "Log.h"

#include <assert.h>

namespace {
	// White player will move South
	const int s_south = 1;

	// Black player moves North.
	const int s_north= -1;

	const int s_west = -1;
	const int s_east = 1;
}

Game::Game()
	: m_boardData(s_boardSize, std::vector<PieceDisplayType>(s_boardSize, EMPTY))
	, m_pendingMoveLauncher(new CheckersMoveLauncher(std::bind(&Game::OnMoveSelectionFinalized, this)))
	, m_whitePlayerTurn(true)
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

	// Row and column are flipped in the UI as opposed to our data.
	return std::make_pair(col, row);
}

void Game::OnMoveSelectionEvent(const BoardIndex& boardIndex)
{
	assert(IsValidBoardIndex(boardIndex));

	// Move source must contain a piece.
	if (!m_pendingMoveLauncher->IsSourceSet() && !ContainsPiece(boardIndex))
	{
		LOG_DEBUG_CONSOLE("Error: Move source does not contain a piece: "
			+ BoardIndexToString(boardIndex));
		return;
	}

	m_pendingMoveLauncher->HandleMoveSelected(boardIndex);
}

void Game::OnMoveSelectionFinalized()
{
	const BoardIndex& moveSource = m_pendingMoveLauncher->GetMoveSource();
	const BoardIndex& moveDestination = m_pendingMoveLauncher->GetMoveDestination();

	LOG_DEBUG_CONSOLE("Info: Attempting to move " + BoardIndexToString(moveSource) + " to "
		+ BoardIndexToString(moveDestination));

	// TODO: IsLegalMove
		// Move()

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
	PopulateLegalCurrentMoves();
}

void Game::PopulateLegalCurrentMoves()
{
	for (int row = 0; row < s_boardSize; ++row)
	{
		for (int col = 0; col < s_boardSize; ++col)
		{
			BoardIndex currentIndex = std::make_pair(row, col);
			PieceDisplayType currentPiece = m_boardData[row][col];
			if (ContainsPiece(currentIndex) && IsPieceOfCurrentPlayer(currentPiece))
			{
				EvaluatePossibleMovesForIndex(currentIndex);
			}
		}
	}
}

bool Game::IsValidBoardIndex(const BoardIndex& boardIndex) const
{
	int row = boardIndex.first;
	int col = boardIndex.second;

	bool isValid = row >= 0 && row < s_boardSize && col >= 0 && col < s_boardSize;

	if (!isValid)
	{
		LOG_DEBUG_CONSOLE("Warning: attempt to access invalid index " + BoardIndexToString(boardIndex));
	}

	return isValid;
}

bool Game::IsLegalMove(const CheckersMove& move) const
{
	// TODO: Return whether this move appears in the jump list or the move list.

	return true;
}

bool Game::ContainsPiece(const BoardIndex& boardIndex) const
{
	return IsValidBoardIndex(boardIndex)
		&& m_boardData[boardIndex.first][boardIndex.second] != EMPTY;
}

bool Game::IsPieceOfCurrentPlayer(PieceDisplayType piece) const
{
	return m_whitePlayerTurn && (piece == WHITE || piece == WHITE_KING)
		|| !m_whitePlayerTurn && (piece == BLACK || piece == BLACK_KING);
}

void Game::EvaluatePossibleMovesForIndex(const BoardIndex& boardIndex)
{
	LOG_DEBUG_CONSOLE("Info: Evaluating moves for " + BoardIndexToString(boardIndex));

	switch (m_boardData[boardIndex.first][boardIndex.second])
	{
	case BLACK:
		AddValidMovesForDirection(boardIndex, s_north, s_west);
		AddValidMovesForDirection(boardIndex, s_north, s_east);
		break;
	case WHITE:
		AddValidMovesForDirection(boardIndex, s_south, s_west);
		AddValidMovesForDirection(boardIndex, s_south, s_east);
		break;
	case BLACK_KING:
	case WHITE_KING:
		AddValidMovesForDirection(boardIndex, s_north, s_west);
		AddValidMovesForDirection(boardIndex, s_north, s_east);
		AddValidMovesForDirection(boardIndex, s_south, s_west);
		AddValidMovesForDirection(boardIndex, s_south, s_east);
		break;
	case EMPTY:
	default:
		break;
	}
}

void Game::AddValidMovesForDirection(const BoardIndex& currentPosition, int verticalDirection,
	int horizontalDirection)
{
	BoardIndex newPosition = std::make_pair(currentPosition.first + verticalDirection,
		currentPosition.second + horizontalDirection);

	if (!IsValidBoardIndex(newPosition))
		return;

	// Normal move.
	if (!ContainsPiece(newPosition))
	{
		LOG_DEBUG_CONSOLE("Info: Added Valid Destination: " + BoardIndexToString(newPosition));
		m_legalDestinations.push_back(GetCheckersMove(currentPosition, newPosition));
	}

	// Possibly a jump
	else if (!IsPieceOfCurrentPlayer(GetPieceForIndex(newPosition)))
	{
		AddValidJumpForDirection(currentPosition, verticalDirection, horizontalDirection);
	}
}

void Game::AddValidJumpForDirection(const BoardIndex& currentPosition, int verticalDirection,
	int horizontalDirection)
{
	// Look ahead an extra space in in the same direction.
	BoardIndex possibleJump = std::make_pair(currentPosition.first + (verticalDirection * 2),
		currentPosition.second + (horizontalDirection * 2));

	if (!IsValidBoardIndex(possibleJump))
		return;

	if (!ContainsPiece(possibleJump))
	{
		LOG_DEBUG_CONSOLE("Info: Added Valid Jump Destination: " + BoardIndexToString(possibleJump));
		m_legalJumpDestinations.push_back(GetCheckersMove(currentPosition, possibleJump));
	}
}

std::string Game::BoardIndexToString(const BoardIndex& index) const
{
	return std::string(std::to_string(index.first) +  " , ") + std::to_string(index.second);
}

PieceDisplayType Game::GetPieceForIndex(const BoardIndex& index) const
{
	return m_boardData[index.first][index.second];
}

CheckersMove Game::GetCheckersMove(const BoardIndex& sourceIndex, const BoardIndex& destinationIndex) const
{
	return std::make_pair(sourceIndex, destinationIndex);
}

void Game::MovePiece()
{
	// TODO: Move the piece.
	OnPieceMoved();
}

void Game::OnPieceMoved()
{
	SwitchTurns();
	PopulateLegalCurrentMoves();
}

void Game::SwitchTurns()
{
	m_whitePlayerTurn = !m_whitePlayerTurn;
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
	: m_checkersMove(BoardIndex(-1,-1), BoardIndex(-1,-1))
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
		m_checkersMove.first = move;
		m_isSourceSet = true;
	}
	else
	{
		// Next is the destination.
		m_checkersMove.second = move;
		m_isDestinationSet = true;

		// Ready to move, launch!
		m_launchMoveCallback();
	}
}

const BoardIndex& CheckersMoveLauncher::GetMoveSource() const
{
	return m_checkersMove.first;
}

const BoardIndex& CheckersMoveLauncher::GetMoveDestination() const
{
	return m_checkersMove.second;
}

bool CheckersMoveLauncher::IsSourceSet() const
{
	return m_isSourceSet;
}
