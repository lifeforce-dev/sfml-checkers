//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "CheckersTypes.h"

#include <memory>
#include <functional>

class CheckersMoveLauncher;

// Source to destination.
typedef std::pair<BoardIndex, BoardIndex> CheckersMove;

class Game
{
public:
	Game();
	~Game();

	const BoardData& GetBoardData() { return m_boardData; }

	// Return the correct board index for UI coordinates.
	BoardIndex GetBoardIndexFromRowCol(int row, int col) const;

	// Called in response to the UI reporting that a player made a selection.
	void OnMoveSelectionEvent(const BoardIndex& boardIndex);

	// This is called from the CheckersMoveLauncher when the move destination is selected.
	// This will reset the CheckersMoveLauncher unconditionally, and attempt to move.
	void OnMoveSelectionFinalized();

private:
	// Setup pieces in initial position and other initialization activities.
	void Setup();

	// For the current player, generate a list of all legal move possibilities.
	void PopulateLegalCurrentMoves();

	// Returns whether the index is a valid board index.
	bool IsValidBoardIndex(const BoardIndex& boardIndex) const;

	// Returns whether the move is legal according to checkers moves.
	bool IsLegalMove(const CheckersMove& move) const;

	// Returns whether the specified index contains a piece.
	bool ContainsPiece(const BoardIndex& boardIndex) const;

	// Returns whether the specified piece belongs to the player whose turn it currently is.
	bool IsPieceOfCurrentPlayer(PieceDisplayType piece) const;

	// This will attempt to add possible moves for a specific piece.
	void EvaluatePossibleMovesForIndex(const BoardIndex& pieceIndex);

	// Given a direction, will add a move to the appropriate list if it is valid.
	void AddValidMovesForDirection(const BoardIndex& currentPosition, int verticalDirection,
		int horizontalDirection);

	// Given a direction, will add a valid jump to the jump list.
	void AddValidJumpForDirection(const BoardIndex& currentPosition, int verticalDirection,
		int horizontalDirection);

	// Will return a string representation of a BaordIndex.
	std::string BoardIndexToString(const BoardIndex& index) const;

	// Returns PieceDisplayType for given index.
	PieceDisplayType GetPieceForIndex(const BoardIndex& index) const;

	// Given a source and a destination will return a CheckersMove.
	CheckersMove GetCheckersMove(const BoardIndex& sourceIndex, const BoardIndex& destinationIndex) const;

	// Will actually move the piece and take care of cleaning up dead pieces/promotions.
	void MovePiece();

	// Called in response to a piece successfully moving.
	void OnPieceMoved();

	// This will toggle player turns
	void SwitchTurns();

	void DEBUG_PRINT_BOARD();

	// Contains every movable index and what type of piece if any is there.
	BoardData m_boardData;

	// Stores the source move and destination move when requested by the player.
	std::unique_ptr<CheckersMoveLauncher> m_pendingMoveLauncher;

	// This list is populated each turn and represents all possible moves of that player.
	std::vector<CheckersMove> m_legalDestinations;

	// If this list is not empty, a move must come from this list.
	std::vector<CheckersMove> m_legalJumpDestinations;

	// Toggle value. If it is not white player's turn, it is black players turn.
	bool m_whitePlayerTurn;
};

class CheckersMoveLauncher
{
public:
	CheckersMoveLauncher(std::function<void()> performMoveCallback);
	~CheckersMoveLauncher();

	// This will through sequential calls, set the source and destination.
	void HandleMoveSelected(const BoardIndex& moveSource);

	const BoardIndex& GetMoveSource() const;
	const BoardIndex& GetMoveDestination() const;

	bool IsSourceSet() const;

private:
	CheckersMove m_checkersMove;

	bool m_isSourceSet;
	bool m_isDestinationSet;

	// This is called when both source and destination moves are set.
	std::function<void()> m_launchMoveCallback;
};
