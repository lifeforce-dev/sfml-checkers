//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"
#include "Log.h"

#include <assert.h>
#include <algorithm>

namespace {
	// White player will move South
	const int s_south = 1;

	// Black player moves North.
	const int s_north= -1;

	// These are the same for both Black and White players.
	const int s_west = -1;
	const int s_east = 1;

	const int s_moveLength = 1;
	const int s_jumpLength = 2;
}

Game::Game()
	: m_boardData(s_boardSize, std::vector<PieceDisplayType>(s_boardSize, EMPTY))
	, m_pendingMoveLauncher(new CheckersMoveLauncher(std::bind(&Game::OnLaunchMove, this)))
	, m_isWhitePlayerTurn(true)
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

void Game::OnLaunchMove()
{
	const CheckersMove& move(m_pendingMoveLauncher->GetCheckersMove());

	LOG_DEBUG_CONSOLE("Info: Attempting to move " + BoardIndexToString(move.m_moveSource) + " to "
		+ BoardIndexToString(move.m_moveDestination));

	bool isJumpAvailable = !m_legalJumpDestinations.empty();

	if (IsLegalJump(move))
	{
		JumpPiece(move);
	}

	// We are not allowed to move if a jump is available.
	else if (IsLegalMove(move) && !isJumpAvailable)
	{
		MovePiece(move);
	}
	else
	{
		LOG_DEBUG_CONSOLE("Info: Illegal move attempt. ");
	}

	// This move is complete, reset it!
	m_pendingMoveLauncher.reset(new CheckersMoveLauncher(std::bind(&Game::OnLaunchMove, this)));
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

	PopulateLegalTurnMoves();
}

void Game::MovePiece(const CheckersMove& currentMove)
{
	const BoardIndex& source = currentMove.m_moveSource;
	const BoardIndex& destination(currentMove.m_moveDestination);

	// Move the source piece to the destination.
	m_boardData[destination.first][destination.second] = GetPieceForMove(currentMove);

	// Clear previous spot.
	m_boardData[source.first][source.second] = EMPTY;

	SwitchTurns();
}

void Game::JumpPiece(const CheckersMove& currentMove)
{
	const BoardIndex& source = currentMove.m_moveSource;
	const BoardIndex& destination(currentMove.m_moveDestination);

	if (!IsValidBoardIndex(destination) || !IsValidBoardIndex(source))
		return;

	Vector2D direction(currentMove.m_verticalDirection, currentMove.m_horizontalDirection);

	// Piece in between source and destination.
	BoardIndex middleOfJumpIndex = GetTranslatedMove(source, direction.m_y, direction.m_x);

	// Move the source piece to the destination.
	m_boardData[destination.first][destination.second] = GetPieceForMove(currentMove);

	// Clear source spot.
	m_boardData[source.first][source.second] = EMPTY;

	// Capture the piece between source and destination.
	m_boardData[middleOfJumpIndex.first][middleOfJumpIndex.second] = EMPTY;

	m_legalJumpDestinations.clear();

	AddValidJumpsFromJump(currentMove, direction.m_y, direction.m_x);

	// If we have more jumps, we do not switch turns as the player gets to make another jump.
	if (m_legalJumpDestinations.empty())
	{
		// If there are no more valid jumps, then the turn is over.
		SwitchTurns();
	}
}

void Game::SwitchTurns()
{
	// Toggle players
	m_isWhitePlayerTurn = !m_isWhitePlayerTurn;

	// Clear our move lists so we can look for new moves next turn.
	m_legalJumpDestinations.clear();
	m_legalDestinations.clear();

	// Repopulate moves.
	PopulateLegalTurnMoves();
}

void Game::PopulateLegalTurnMoves()
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
	return std::any_of(m_legalDestinations.begin(), m_legalDestinations.end(),
		[&move](const CheckersMove& currentMove)
	{
		return move.m_moveSource == currentMove.m_moveSource
			&& move.m_moveDestination == currentMove.m_moveDestination;
	});
}

bool Game::IsLegalJump(const CheckersMove& move) const
{
	return std::any_of(m_legalJumpDestinations.begin(), m_legalJumpDestinations.end(),
		[&move](const CheckersMove& currentMove)
	{
		return move.m_moveSource == currentMove.m_moveSource
			&& move.m_moveDestination == currentMove.m_moveDestination;
	});
}

bool Game::IsKingableIndex(const BoardIndex& boardIndex) const
{
	// If we are within the first or last row, we are kingable.
	return (boardIndex.first == 0 && boardIndex.second >= 0 && boardIndex.second < s_boardSize)
		|| (boardIndex.first == s_boardSize - 1 && boardIndex.first >= 0 && boardIndex.first < s_boardSize);
}

bool Game::ContainsPiece(const BoardIndex& boardIndex) const
{
	return IsValidBoardIndex(boardIndex)
		&& m_boardData[boardIndex.first][boardIndex.second] != EMPTY;
}

bool Game::ContainsPlayerPiece(const BoardIndex& boardIndex) const
{
	return IsValidBoardIndex(boardIndex)
		&& IsPieceOfCurrentPlayer(m_boardData[boardIndex.first][boardIndex.second]);
}

bool Game::ContainsEnemyPiece(const BoardIndex& boardIndex) const
{
	return IsValidBoardIndex(boardIndex)
		&& m_boardData[boardIndex.first][boardIndex.second] != EMPTY
		&& !IsPieceOfCurrentPlayer(m_boardData[boardIndex.first][boardIndex.second]);
}

bool Game::IsPieceOfCurrentPlayer(PieceDisplayType piece) const
{
	return m_isWhitePlayerTurn && (piece == WHITE || piece == WHITE_KING)
		|| !m_isWhitePlayerTurn && (piece == BLACK || piece == BLACK_KING);
}

void Game::EvaluatePossibleMovesForIndex(const BoardIndex& boardIndex)
{
	LOG_DEBUG_CONSOLE("Info: Evaluating moves for " + BoardIndexToString(boardIndex));

	switch (m_boardData[boardIndex.first][boardIndex.second])
	{
	case BLACK:
		// Look for jumps and move looking North East and North West.
		AddValidMoveForDirection(boardIndex, s_north, s_west);
		AddValidMoveForDirection(boardIndex, s_north, s_east);
		AddValidJumpForDirection(boardIndex, s_north, s_west);
		AddValidJumpForDirection(boardIndex, s_north, s_east);
		break;
	case WHITE:
		// Look for jumps and move looking South East and South West.
		AddValidMoveForDirection(boardIndex, s_south, s_west);
		AddValidJumpForDirection(boardIndex, s_south, s_west);
		AddValidMoveForDirection(boardIndex, s_south, s_east);
		AddValidJumpForDirection(boardIndex, s_south, s_east);
		break;
	case BLACK_KING:
	case WHITE_KING:
		// Look for jumps and moves in all directions.
		AddValidMoveForDirection(boardIndex, s_north, s_west);
		AddValidMoveForDirection(boardIndex, s_north, s_east);
		AddValidMoveForDirection(boardIndex, s_south, s_west);
		AddValidMoveForDirection(boardIndex, s_south, s_east);
		AddValidJumpForDirection(boardIndex, s_north, s_west);
		AddValidJumpForDirection(boardIndex, s_north, s_east);
		AddValidJumpForDirection(boardIndex, s_south, s_west);
		AddValidJumpForDirection(boardIndex, s_south, s_east);
		break;
	case EMPTY:
	default:
		break;
	}
}

void Game::AddValidMoveForDirection(const BoardIndex& currentPosition, int verticalDirection,
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
		m_legalDestinations.push_back(CreateCheckersMove(currentPosition, newPosition));
	}
}

void Game::AddValidJumpForDirection(const BoardIndex& currentPosition, int verticalDirection,
	int horizontalDirection)
{
	// Two spaces in the same direction we are headed.
	BoardIndex jumpPosition = GetTranslatedJump(currentPosition, verticalDirection, horizontalDirection);

	// One space in the same direction we are headed.
	BoardIndex middlePosition = GetTranslatedMove(currentPosition, verticalDirection, horizontalDirection);

	if (!IsValidBoardIndex(jumpPosition) || !IsValidBoardIndex(middlePosition))
		return;

	// If the location contains a piece that is not mine
	if (ContainsEnemyPiece(middlePosition) && !ContainsPiece(jumpPosition))
	{
		LOG_DEBUG_CONSOLE("Info: Added Valid Jump Destination: " + BoardIndexToString(jumpPosition));
		m_legalJumpDestinations.push_back(CreateCheckersMove(currentPosition, jumpPosition));
	}
}

void Game::AddValidJumpsFromJump(const CheckersMove& currentMove,
	int verticalDirection, int horizontalDirection)
{
	Vector2D direction(currentMove.m_verticalDirection, currentMove.m_horizontalDirection);
	const BoardIndex& destination(currentMove.m_moveDestination);

	// After the jump, we need to look east or west in the same direction to see if we can jump again.
	BoardIndex lookaheadHorizontal = GetTranslatedMove(currentMove.m_moveDestination,
		direction.m_y, direction.m_x);

	BoardIndex lookaheadHorizontalInverted = GetTranslatedMove(currentMove.m_moveDestination,
		direction.m_y, direction.m_x * -1);

	// TODO: I multiply by -1 here and in the AddValidJumpForDirection call. Refactor.
	BoardIndex lookaheadVerticalInverted = GetTranslatedMove(currentMove.m_moveDestination,
		direction.m_y * -1, direction.m_x);

	if (IsValidBoardIndex(lookaheadHorizontal) &&
		!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadHorizontal)))
	{
		// Look in the same vertical direction and horizontal direction.
		AddValidJumpForDirection(destination, direction.m_y, direction.m_x);
	}

	if (IsValidBoardIndex(lookaheadHorizontalInverted) &&
		!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadHorizontalInverted)))
	{
		// Look in the same vertical direction and opposite horizontal direction.
		AddValidJumpForDirection(destination, direction.m_y, direction.m_x * -1);
	}

	if (GetPieceForIndex(currentMove.m_moveDestination) == BLACK_KING
		|| GetPieceForIndex(currentMove.m_moveDestination) == WHITE_KING)
	{
		if (IsValidBoardIndex(lookaheadVerticalInverted) &&
			!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadVerticalInverted)))
		{
			// Look in the same vertical direction and opposite horizontal direction.
			AddValidJumpForDirection(destination, direction.m_y * -1, direction.m_x);
		}
	}
}

BoardIndex Game::GetTranslatedMove(const BoardIndex& source, int verticalDirection,
	int horizontalDirection)
{
	return BoardIndex(source.first + s_moveLength * verticalDirection,
		source.second + s_moveLength * horizontalDirection);
}

BoardIndex Game::GetTranslatedJump(const BoardIndex& source, int verticalDirection, int horizontalDirection)
{
	return BoardIndex(source.first + s_jumpLength * verticalDirection,
		source.second + s_jumpLength * horizontalDirection);
}

std::string Game::BoardIndexToString(const BoardIndex& index) const
{
	return std::string(std::to_string(index.first) +  " , ") + std::to_string(index.second);
}

PieceDisplayType Game::GetPieceForIndex(const BoardIndex& index) const
{
	return m_boardData[index.first][index.second];
}

PieceDisplayType Game::GetPieceForMove(const CheckersMove& move) const
{
	switch (GetPieceForIndex(move.m_moveSource))
	{
	case WHITE:
		if (IsKingableIndex(move.m_moveDestination))
			return WHITE_KING;
		return WHITE;
		break;
	case BLACK:
		if (IsKingableIndex(move.m_moveDestination))
			return BLACK_KING;
		return BLACK;
		break;
	case BLACK_KING:
		return BLACK_KING;
		break;
	case WHITE_KING:
		return WHITE_KING;
		break;
	default:
		return INVALID;
		break;
	}
}

CheckersMove Game::CreateCheckersMove(const BoardIndex& sourceIndex,
	const BoardIndex& destinationIndex) const
{
	return CheckersMove(sourceIndex, destinationIndex);
}

//---------------------------------------------------------------

CheckersMove::CheckersMove(BoardIndex source, BoardIndex destination, int verticalDirection,
	int horizontalDirection)
	: m_moveSource(source)
	, m_moveDestination(destination)
	, m_verticalDirection(verticalDirection)
	, m_horizontalDirection(horizontalDirection)
	, m_moveLength(0)
{
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
		m_checkersMove.m_moveSource = move;
		m_isSourceSet = true;
	}
	else
	{
		// Next is the destination.
		m_checkersMove.m_moveDestination = move;

		SetMoveDistance();
		SetMoveLength();

		if (m_checkersMove.m_moveLength == 0)
		{
			// We didn't actually move anywhere.
			m_isSourceSet = false;
			return;
		}

		// Determine the direction of the move and set it.
		SetMoveDirection();

		m_isDestinationSet = true;

		// Ready to move, launch!
		m_launchMoveCallback();
	}
}

const BoardIndex& CheckersMoveLauncher::GetMoveSource() const
{
	return m_checkersMove.m_moveSource;
}

const BoardIndex& CheckersMoveLauncher::GetMoveDestination() const
{
	return m_checkersMove.m_moveDestination;
}

const CheckersMove& CheckersMoveLauncher::GetCheckersMove() const
{
	return m_checkersMove;
}

bool CheckersMoveLauncher::IsSourceSet() const
{
	return m_isSourceSet;
}

void CheckersMoveLauncher::SetMoveDirection()
{
	int distanceCol = m_checkersMove.m_columnDistance;
	int distanceRow = m_checkersMove.m_rowDistance;

	m_checkersMove.m_verticalDirection = distanceRow / m_checkersMove.m_moveLength;
	m_checkersMove.m_horizontalDirection = distanceCol / m_checkersMove.m_moveLength;
}

void CheckersMoveLauncher::SetMoveLength()
{
	int distanceCol = m_checkersMove.m_columnDistance;
	int distanceRow = m_checkersMove.m_rowDistance;

	// Normalize.
	m_checkersMove.m_moveLength = static_cast<int>(std::sqrt(distanceCol * distanceCol
		+ distanceRow * distanceRow));
}

void CheckersMoveLauncher::SetMoveDistance()
{
	int sourceRow = m_checkersMove.m_moveSource.first;
	int sourceCol = m_checkersMove.m_moveSource.second;
	int destinationRow = m_checkersMove.m_moveDestination.first;
	int destinationCol = m_checkersMove.m_moveDestination.second;

	m_checkersMove.m_rowDistance = destinationRow - sourceRow;
	m_checkersMove.m_columnDistance = destinationCol - sourceCol;
}
