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
	, m_moveHelper(new CheckersMoveHelper(this))
	, m_isWhitePlayerTurn(true)
{
	Setup();
}

Game::~Game()
{
}

BoardIndex Game::GetBoardIndexFromRowCol(int row, int col) const
{
	if (!IsValidBoardIndex(BoardIndex(row, col)))
		return BoardIndex();

	// Row and column are flipped in the UI as opposed to our data.
	return BoardIndex(col, row);
}

void Game::OnMoveSelectionEvent(const BoardIndex& boardIndex)
{
	assert(IsValidBoardIndex(boardIndex));

	m_moveHelper->HandleMoveSelected(boardIndex);
}

void Game::OnLaunchMove(const CheckersMove& move)
{
	LOG_DEBUG_CONSOLE("Info: Attempting to move " + BoardIndexToString(move.GetSource()) + " to "
		+ BoardIndexToString(move.GetDestination()));
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
	const BoardIndex& source = currentMove.GetSource();
	const BoardIndex& destination(currentMove.GetDestination());

	// Move the source piece to the destination.
	m_boardData[destination.row][destination.col] = GetPieceForMove(currentMove);

	// Clear previous spot.
	m_boardData[source.row][source.col] = EMPTY;

	SwitchTurns();
}

void Game::JumpPiece(const CheckersMove& currentMove)
{
	const BoardIndex& source = currentMove.GetSource();
	const BoardIndex& destination(currentMove.GetDestination());

	if (!IsValidBoardIndex(destination) || !IsValidBoardIndex(source))
		return;

	Vector2D direction = currentMove.GetDirection();

	// Piece in between source and destination.
	BoardIndex middleOfJumpIndex = GetTranslatedMove(source, direction.y, direction.x);

	// Move the source piece to the destination.
	m_boardData[destination.row][destination.col] = GetPieceForMove(currentMove);

	// Clear source spot.
	m_boardData[source.row][source.col] = EMPTY;

	// Capture the piece between source and destination.
	m_boardData[middleOfJumpIndex.row][middleOfJumpIndex.col] = EMPTY;

	m_legalJumpDestinations.clear();

	AddValidJumpsFromJump(currentMove, direction.y, direction.x);

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
			BoardIndex currentIndex = BoardIndex(row, col);
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
	int row = boardIndex.row;
	int col = boardIndex.col;

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
		return move.GetSource() == currentMove.GetSource()
			&& move.GetDestination() == currentMove.GetDestination();
	});
}

bool Game::IsLegalJump(const CheckersMove& move) const
{
	return std::any_of(m_legalJumpDestinations.begin(), m_legalJumpDestinations.end(),
		[&move](const CheckersMove& currentMove)
	{
		return move.GetSource() == currentMove.GetSource()
			&& move.GetDestination() == currentMove.GetDestination();
	});
}

bool Game::IsKingableIndex(const BoardIndex& boardIndex) const
{
	// If we are within the first or last row, we are kingable.
	return (boardIndex.row == 0 && boardIndex.col >= 0 && boardIndex.col < s_boardSize)
		|| (boardIndex.row == s_boardSize - 1 && boardIndex.row >= 0 && boardIndex.row < s_boardSize);
}

bool Game::ContainsPiece(const BoardIndex& boardIndex) const
{
	return IsValidBoardIndex(boardIndex)
		&& m_boardData[boardIndex.row][boardIndex.col] != EMPTY;
}

bool Game::ContainsPlayerPiece(const BoardIndex& boardIndex) const
{
	return IsValidBoardIndex(boardIndex)
		&& IsPieceOfCurrentPlayer(m_boardData[boardIndex.row][boardIndex.col]);
}

bool Game::ContainsEnemyPiece(const BoardIndex& boardIndex) const
{
	return IsValidBoardIndex(boardIndex)
		&& m_boardData[boardIndex.row][boardIndex.col] != EMPTY
		&& !IsPieceOfCurrentPlayer(m_boardData[boardIndex.row][boardIndex.col]);
}

bool Game::IsPieceOfCurrentPlayer(PieceDisplayType piece) const
{
	return m_isWhitePlayerTurn && (piece == WHITE || piece == WHITE_KING)
		|| !m_isWhitePlayerTurn && (piece == BLACK || piece == BLACK_KING);
}

void Game::EvaluatePossibleMovesForIndex(const BoardIndex& boardIndex)
{
	LOG_DEBUG_CONSOLE("Info: Evaluating moves for " + BoardIndexToString(boardIndex));

	switch (m_boardData[boardIndex.row][boardIndex.col])
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
	BoardIndex newPosition = BoardIndex(currentPosition.row + verticalDirection,
		currentPosition.col + horizontalDirection);

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
	Vector2D direction = currentMove.GetDirection();
	const BoardIndex& destination(currentMove.GetDestination());

	Vector2D invertedDirection(direction.y * -1, direction.x * -1);

	// After the jump, we need to look east or west in the same direction to see if we can jump again.
	BoardIndex lookaheadHorizontal = GetTranslatedMove(currentMove.GetDestination(),
		direction.y, direction.x);

	BoardIndex lookaheadHorizontalInverted = GetTranslatedMove(currentMove.GetDestination(),
		direction.y, invertedDirection.x);

	if (IsValidBoardIndex(lookaheadHorizontal) &&
		!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadHorizontal)))
	{
		// Look in the same vertical direction and horizontal direction.
		AddValidJumpForDirection(destination, direction.y, direction.x);
	}

	if (IsValidBoardIndex(lookaheadHorizontalInverted) &&
		!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadHorizontalInverted)))
	{
		// Look in the same vertical direction and opposite horizontal direction.
		AddValidJumpForDirection(destination, direction.y, invertedDirection.x);
	}

	if (GetPieceForIndex(currentMove.GetDestination()) == BLACK_KING
		|| GetPieceForIndex(currentMove.GetDestination()) == WHITE_KING)
	{
		BoardIndex lookaheadVerticalInverted = GetTranslatedMove(currentMove.GetDestination(),
			invertedDirection.y, direction.x);

		if (IsValidBoardIndex(lookaheadVerticalInverted) &&
			!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadVerticalInverted)))
		{
			// Look in the opposite vertical direction and same horizontal direction.
			AddValidJumpForDirection(destination, invertedDirection.y, direction.x);
		}
	}
}

BoardIndex Game::GetTranslatedMove(const BoardIndex& source, int verticalDirection,
	int horizontalDirection)
{
	return BoardIndex(source.row + s_moveLength * verticalDirection,
		source.col + s_moveLength * horizontalDirection);
}

BoardIndex Game::GetTranslatedJump(const BoardIndex& source, int verticalDirection, int horizontalDirection)
{
	return BoardIndex(source.row + s_jumpLength * verticalDirection,
		source.col + s_jumpLength * horizontalDirection);
}

std::string Game::BoardIndexToString(const BoardIndex& index) const
{
	return std::string(std::to_string(index.row) +  " , ") + std::to_string(index.col);
}

PieceDisplayType Game::GetPieceForIndex(const BoardIndex& index) const
{
	return m_boardData[index.row][index.col];
}

PieceDisplayType Game::GetPieceForMove(const CheckersMove& move) const
{
	switch (GetPieceForIndex(move.GetSource()))
	{
	case WHITE:
		if (IsKingableIndex(move.GetDestination()))
			return WHITE_KING;
		return WHITE;
		break;
	case BLACK:
		if (IsKingableIndex(move.GetDestination()))
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

CheckersMove::CheckersMove(BoardIndex source, BoardIndex destination)
	: m_source(source)
	, m_destination(destination)
	, m_length(0)
{
}

void CheckersMove::SetDestination(BoardIndex destination)
{
	m_destination = destination;
	UpdateMoveDistance();
	UpdateMoveLength();
	UpdateMoveDirection();
}

void CheckersMove::UpdateMoveDistance()
{
	// dx = x2 - x1
	m_distance.x = m_destination.row - m_source.row;
	m_distance.y = m_destination.col - m_source.col;
}

void CheckersMove::UpdateMoveLength()
{
	// Normalize.
	m_length = static_cast<int>(std::sqrt(m_distance.y * m_distance.y +
		m_distance.x * m_distance.x));
}

void CheckersMove::UpdateMoveDirection()
{
	// 0,0 is top left. We want x to be row (up and down) and y to be col.
	m_direction.x = m_distance.y / m_length;
	m_direction.y = m_distance.x / m_length;
}

//---------------------------------------------------------------

CheckersMoveHelper::CheckersMoveHelper(Game* game)
	: m_game(game)
{
}

CheckersMoveHelper::~CheckersMoveHelper()
{
}

void CheckersMoveHelper::HandleMoveSelected(const BoardIndex& move)
{
	if (!m_isSelectingMove)
	{
		m_isSelectingMove = true;
		m_checkersMove.SetSource(move);
	}
	else
	{
		m_checkersMove.SetDestination(move);

		if (m_checkersMove.GetLength() > 0)
		{
			// Ready to move, launch!
			m_game->OnLaunchMove(m_checkersMove);
		}

		// Clear state.
		m_checkersMove = CheckersMove();
		m_isSelectingMove = false;
	}
}

const CheckersMove& CheckersMoveHelper::GetCheckersMove() const
{
	return m_checkersMove;
}
