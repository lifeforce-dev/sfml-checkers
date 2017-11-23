//---------------------------------------------------------------
//
// CheckersMoveHelper.h
//

#include "CheckersMoveHelper.h"

#include "Game.h"

namespace Checkers {

//========================================================================

void CheckersMoveHelper::HandlePositionSelected(const Position& position)
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

const CheckersMove& CheckersMoveHelper::GetCheckersMove() const
{
	return m_checkersMove;
}

//========================================================================

} // namespace Checkers
