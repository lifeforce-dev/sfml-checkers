//---------------------------------------------------------------
//
// CheckersMoveHelper.h
//

#pragma once

#include "CheckersTypes.h"

namespace Checkers {

//========================================================================

class Game;

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
	void HandlePositionSelected(const Position& position);

	const CheckersMove& GetCheckersMove() const;

private:
	Game* m_game;
	CheckersMove m_checkersMove;

	bool m_isSelectingMove = false;
};

//========================================================================

} // namespace Checkers
