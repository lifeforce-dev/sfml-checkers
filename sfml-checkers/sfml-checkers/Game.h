//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "CheckersTypes.h"

#include <memory>
#include <functional>

class CheckersMoveLauncher;

class Game
{
public:
	Game();
	~Game();

	const BoardData& GetBoardData() { return m_boardData; }
	BoardIndex GetBoardIndexFromRowCol(int row, int col) const;

	void OnMoveSelectionEvent(const BoardIndex& boardIndex);

	// This is called from the CheckersMoveLauncher when the move destination is selected.
	// This will reset the CheckersMoveLauncher unconditionally, and attempt to move.
	void OnMoveSelectionFinalized();

private:
	void Setup();
	bool IsValidBoardIndex(const BoardIndex& boardIndex) const;

	void DEBUG_PRINT_BOARD();

	// Contains every movable index and what type of piece if any is there.
	BoardData m_boardData;

	// Stores the source move and destination move when requested by the player.
	std::unique_ptr<CheckersMoveLauncher> m_pendingMoveLauncher;

	// This list is populated each turn and represents all possible moves of that player.
	std::vector<BoardIndex> m_possibleMoveDestinations;
};

//---------------------------------------------------------------

class CheckersMoveLauncher
{
public:
	CheckersMoveLauncher(std::function<void()> performMoveCallback);
	~CheckersMoveLauncher();

	// This will through sequential calls, set the source and destination.
	void HandleMoveSelected(const BoardIndex& moveSource);

	const BoardIndex& GetMoveSource() const;
	const BoardIndex& GetMoveDestination() const;

private:
	BoardIndex m_moveSource;
	BoardIndex m_moveDestination;

	bool m_isSourceSet;
	bool m_isDestinationSet;

	// This is called when both source and destination moves are set.
	std::function<void()> m_launchMoveCallback;
};
