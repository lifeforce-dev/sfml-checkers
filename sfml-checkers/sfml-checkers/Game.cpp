//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"
#include "Log.h"

#include <assert.h>
#include <algorithm>

namespace Checkers {

//========================================================================

namespace
{
	// White player will move South
	const int s_south = 1;

	// Black player moves North.
	const int s_north= -1;

	// These are the same for both Black and White players.
	const int s_west = -1;
	const int s_east = 1;

	const int s_moveLength = 1;
	const int s_jumpLength = 2;

	const int s_boardSize = 8;

	const Range s_indexRange(0, s_boardSize - 1);
}

//------------------------------------------------------------------------

// Encapsulates a piece move from one position to another.
class CheckersMove
{
public:
	CheckersMove() {}
	CheckersMove::CheckersMove(Position source, Position destination)
		: m_source(source)
		, m_destination(destination)
		, m_length(0)
	{
	}
	void SetSource(Position source) { m_source = source; }

	void SetDestination(Position destination)
	{
		m_destination = destination;
		UpdateMoveDistance();
		UpdateMoveLength();
		UpdateMoveDirection();
	}

	Vector2D GetDirection() const { return m_direction; }
	Vector2D GetDistance() const { return m_distance; }
	int GetLength() const { return m_length; }
	const Position& GetSource() const { return m_source; }
	const Position& GetDestination() const { return m_destination; }

private:
	Position m_source;
	Position m_destination;

	Vector2D m_direction;
	Vector2D m_distance;
	int m_length = 0;

	void UpdateMoveDistance()
	{
		// dx = x2 - x1
		m_distance.x = m_destination.row - m_source.row;
		m_distance.y = m_destination.col - m_source.col;
	}

	void UpdateMoveLength()
	{
		// Normalize.
		m_length = static_cast<int>(std::sqrt(m_distance.y * m_distance.y +
			m_distance.x * m_distance.x));
	}

	void UpdateMoveDirection()
	{
		if (m_length == 0)
		{
			// We didn't move anywhere, so we can't have a direction.
			return;
		}
		// 0,0 is top left. We want x to be row (up and down) and y to be col.
		m_direction.x = m_distance.y / m_length;
		m_direction.y = m_distance.x / m_length;
	}

};

//------------------------------------------------------------------------

// Helper class for managing and executing a move
class CheckersMoveHelper
{
public:
	CheckersMoveHelper(Game* game) : m_game(game) { }
	~CheckersMoveHelper() { }

	// Attempts to set the source and destination of a move, and then execute it.
	void HandlePositionSelected(const Position& position)
	{
		if (!m_isSelectingMove)
		{
			if (m_game->ContainsPiece(position))
			{
				m_isSelectingMove = true;
				m_checkersMove.SetSource(position);
			}
		}
		else
		{
			if (position != m_checkersMove.GetSource())
			{
				m_checkersMove.SetDestination(position);
			}

			// Ready to move, launch!
			m_game->OnLaunchMove(m_checkersMove);

			// Clear state.
			m_checkersMove = CheckersMove();
			m_isSelectingMove = false;
		}
	}

	const CheckersMove& GetCheckersMove() const
	{
		return m_checkersMove;
	}

private:
	Game* m_game;
	CheckersMove m_checkersMove;

	// This is called when both source and destination moves are set.
	std::function<void()> m_launchMoveCallback;

	bool m_isSelectingMove = false;
};

//------------------------------------------------------------------------

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

int Game::GetBoardSize()
{
	return s_boardSize;
}

Position Game::GetPositionFromRowCol(int row, int col) const
{
	if (!IsValidPosition(Position(row, col)))
		return Position();

	// Row and column are flipped in the UI as opposed to our data.
	return Position(col, row);
}

void Game::OnMoveSelectionEvent(const Position& position)
{
	assert(IsValidPosition(position));
	m_moveHelper->HandlePositionSelected(position);
}

void Game::OnLaunchMove(const CheckersMove& move)
{
	LOG_DEBUG_CONSOLE("Info: Attempting to move " + PositionToString(move.GetSource()) + " to "
		+ PositionToString(move.GetDestination()));
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
	const Position& source = currentMove.GetSource();
	const Position& destination(currentMove.GetDestination());

	// Move the source piece to the destination.
	m_boardData[destination.row][destination.col] = GetPieceForMove(currentMove);

	// Clear previous spot.
	m_boardData[source.row][source.col] = EMPTY;

	SwitchTurns();
}

void Game::JumpPiece(const CheckersMove& currentMove)
{
	const Position& source = currentMove.GetSource();
	const Position& destination(currentMove.GetDestination());

	if (!IsValidPosition(destination) || !IsValidPosition(source))
		return;

	Vector2D direction = currentMove.GetDirection();

	// Piece in between source and destination.
	Position capturedPiecePosition = GetTranslatedMove(source, direction.y, direction.x);

	// Move the source piece to the destination.
	m_boardData[destination.row][destination.col] = GetPieceForMove(currentMove);

	// Clear source spot.
	m_boardData[source.row][source.col] = EMPTY;

	// Capture the captured piece's spot.
	m_boardData[capturedPiecePosition.row][capturedPiecePosition.col] = EMPTY;

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
	std::string playerTurn = m_isWhitePlayerTurn ? "White's" : "Black's";
	LOG_DEBUG_CONSOLE(playerTurn + " turn");

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
			Position currentIndex = Position(row, col);
			PieceDisplayType currentPiece = m_boardData[row][col];
			if (ContainsPiece(currentIndex) && IsPieceOfCurrentPlayer(currentPiece))
			{
				EvaluatePossibleMovesForIndex(currentIndex);
			}
		}
	}
}

bool Game::IsValidPosition(const Position& position) const
{
	return s_indexRange.Contains(position.row) && s_indexRange.Contains(position.col);
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

bool Game::IsKingableIndex(const Position& position) const
{
	// If we are within the first or last row, we are kingable.
	return IsValidPosition(position) && (position.row == 0 ||
		position.row == s_boardSize - 1);
}

bool Game::ContainsPiece(const Position& position) const
{
	return IsValidPosition(position)
		&& m_boardData[position.row][position.col] != EMPTY;
}

bool Game::ContainsPlayerPiece(const Position& position) const
{
	return IsValidPosition(position)
		&& IsPieceOfCurrentPlayer(m_boardData[position.row][position.col]);
}

bool Game::ContainsEnemyPiece(const Position& position) const
{
	return IsValidPosition(position)
		&& m_boardData[position.row][position.col] != EMPTY
		&& !IsPieceOfCurrentPlayer(m_boardData[position.row][position.col]);
}

bool Game::IsPieceOfCurrentPlayer(PieceDisplayType piece) const
{
	return m_isWhitePlayerTurn && (piece == WHITE || piece == WHITE_KING)
		|| !m_isWhitePlayerTurn && (piece == BLACK || piece == BLACK_KING);
}

void Game::EvaluatePossibleMovesForIndex(const Position& position)
{
	LOG_DEBUG_CONSOLE("Info: Evaluating moves for " + PositionToString(position));

	switch (m_boardData[position.row][position.col])
	{
	case BLACK:
		// Look for jumps and move looking North East and North West.
		AddValidMoveForDirection(position, s_north, s_west);
		AddValidMoveForDirection(position, s_north, s_east);
		AddValidJumpForDirection(position, s_north, s_west);
		AddValidJumpForDirection(position, s_north, s_east);
		break;
	case WHITE:
		// Look for jumps and move looking South East and South West.
		AddValidMoveForDirection(position, s_south, s_west);
		AddValidJumpForDirection(position, s_south, s_west);
		AddValidMoveForDirection(position, s_south, s_east);
		AddValidJumpForDirection(position, s_south, s_east);
		break;
	case BLACK_KING:
	case WHITE_KING:
		// Look for jumps and moves in all directions.
		AddValidMoveForDirection(position, s_north, s_west);
		AddValidMoveForDirection(position, s_north, s_east);
		AddValidMoveForDirection(position, s_south, s_west);
		AddValidMoveForDirection(position, s_south, s_east);
		AddValidJumpForDirection(position, s_north, s_west);
		AddValidJumpForDirection(position, s_north, s_east);
		AddValidJumpForDirection(position, s_south, s_west);
		AddValidJumpForDirection(position, s_south, s_east);
		break;
	case EMPTY:
	default:
		break;
	}
}

void Game::AddValidMoveForDirection(const Position& currentPosition, int verticalDirection,
	int horizontalDirection)
{
	Position newPosition = Position(currentPosition.row + verticalDirection,
		currentPosition.col + horizontalDirection);

	if (!IsValidPosition(newPosition))
		return;

	// Normal move.
	if (!ContainsPiece(newPosition))
	{
		LOG_DEBUG_CONSOLE("Info: Added Valid Destination: " + PositionToString(newPosition));
		m_legalDestinations.push_back(CreateCheckersMove(currentPosition, newPosition));
	}
}

void Game::AddValidJumpForDirection(const Position& currentPosition, int verticalDirection,
	int horizontalDirection)
{
	// Two spaces in the same direction we are headed.
	Position jumpPosition = GetTranslatedJump(currentPosition, verticalDirection, horizontalDirection);

	// One space in the same direction we are headed.
	Position middlePosition = GetTranslatedMove(currentPosition, verticalDirection, horizontalDirection);

	if (!IsValidPosition(jumpPosition) || !IsValidPosition(middlePosition))
		return;

	// If the location contains a piece that is not mine
	if (ContainsEnemyPiece(middlePosition) && !ContainsPiece(jumpPosition))
	{
		LOG_DEBUG_CONSOLE("Info: Added Valid Jump Destination: " + PositionToString(jumpPosition));
		m_legalJumpDestinations.push_back(CreateCheckersMove(currentPosition, jumpPosition));
	}
}

void Game::AddValidJumpsFromJump(const CheckersMove& currentMove,
	int verticalDirection, int horizontalDirection)
{
	Vector2D direction = currentMove.GetDirection();
	const Position& destination(currentMove.GetDestination());

	Vector2D invertedDirection(direction.y * -1, direction.x * -1);

	// After the jump, we need to look east or west in the same direction to see if we can jump again.
	Position lookaheadHorizontal = GetTranslatedMove(currentMove.GetDestination(),
		direction.y, direction.x);

	Position lookaheadHorizontalInverted = GetTranslatedMove(currentMove.GetDestination(),
		direction.y, invertedDirection.x);

	if (IsValidPosition(lookaheadHorizontal) &&
		!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadHorizontal)))
	{
		// Look in the same vertical direction and horizontal direction.
		AddValidJumpForDirection(destination, direction.y, direction.x);
	}

	if (IsValidPosition(lookaheadHorizontalInverted) &&
		!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadHorizontalInverted)))
	{
		// Look in the same vertical direction and opposite horizontal direction.
		AddValidJumpForDirection(destination, direction.y, invertedDirection.x);
	}

	if (GetPieceForIndex(currentMove.GetDestination()) == BLACK_KING
		|| GetPieceForIndex(currentMove.GetDestination()) == WHITE_KING)
	{
		Position lookaheadVerticalInverted = GetTranslatedMove(currentMove.GetDestination(),
			invertedDirection.y, direction.x);

		if (IsValidPosition(lookaheadVerticalInverted) &&
			!IsPieceOfCurrentPlayer(GetPieceForIndex(lookaheadVerticalInverted)))
		{
			// Look in the opposite vertical direction and same horizontal direction.
			AddValidJumpForDirection(destination, invertedDirection.y, direction.x);
		}
	}
}

Position Game::GetTranslatedMove(const Position& source, int verticalDirection,
	int horizontalDirection)
{
	return Position(source.row + s_moveLength * verticalDirection,
		source.col + s_moveLength * horizontalDirection);
}

Position Game::GetTranslatedJump(const Position& source, int verticalDirection, int horizontalDirection)
{
	return Position(source.row + s_jumpLength * verticalDirection,
		source.col + s_jumpLength * horizontalDirection);
}

std::string Game::PositionToString(const Position& index) const
{
	return std::string(std::to_string(index.row) +  " , ") + std::to_string(index.col);
}

PieceDisplayType Game::GetPieceForIndex(const Position& index) const
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

CheckersMove Game::CreateCheckersMove(const Position& sourceIndex,
	const Position& destinationIndex) const
{
	return CheckersMove(sourceIndex, destinationIndex);
}

//========================================================================

} // namespace Checkers
