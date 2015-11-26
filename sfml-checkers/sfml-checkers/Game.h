//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "CheckersTypes.h"

#include <memory>
#include <functional>

class CheckersMoveLauncher;
struct CheckersMove;

struct Vector2D
{
	Vector2D(int y, int x)
		: m_y(y)
		, m_x(x)
	{
	}

	int m_x;
	int m_y;
};

//---------------------------------------------------------------

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

private:
	// This is called from the CheckersMoveLauncher when the move destination is selected.
	// This will reset the CheckersMoveLauncher unconditionally, and attempt to move.
	void OnLaunchMove();

	// Setup pieces in initial position and other initialization activities.
	void Setup();

	// Will actually move the piece.
	void MovePiece(const CheckersMove& move);

	// Will jump the piece, capture the piece, and look for further jumps.
	void JumpPiece(const CheckersMove& move);

	// This will toggle player turns.
	void SwitchTurns();

	// Reran per turn. Contains all possible moves of a player.
	void PopulateLegalTurnMoves();

	// Returns whether the index is within the bounds of the board.
	bool IsValidBoardIndex(const BoardIndex& boardIndex) const;

	// Returns whether the move is available in the legal move list.
	bool IsLegalMove(const CheckersMove& move) const;

	// Returns whether the move is available in the legal jump list.
	bool IsLegalJump(const CheckersMove& move) const;

	// Returns whether the move results in a kingable position.
	bool IsKingableIndex(const BoardIndex& move) const;

	// Returns whether the specified index contains a piece.
	bool ContainsPiece(const BoardIndex& boardIndex) const;

	// Returns whether the specified index contains a piece of the current player's turn.
	bool ContainsPlayerPiece(const BoardIndex& boardIndex) const;

	// Returns whether the specified index contains a piece of the enemy of the current player's turn.
	bool ContainsEnemyPiece(const BoardIndex& boardIndex) const;

	// Returns whether the specified piece belongs to the player whose turn it currently is.
	bool IsPieceOfCurrentPlayer(PieceDisplayType piece) const;

	// This will attempt to add possible moves for a specific piece.
	void EvaluatePossibleMovesForIndex(const BoardIndex& pieceIndex);

	// Given a direction, will add a move to the appropriate list if it is valid.
	void AddValidMoveForDirection(const BoardIndex& currentPosition, int verticalDirection,
		int horizontalDirection);

	// Given a direction, will add a valid jump to the jump list.
	void AddValidJumpForDirection(const BoardIndex& currentPosition, int verticalDirection,
		int horizontalDirection);

	// Given a jump, will try and assemble the next jump in the chain if one exists.
	void AddValidJumpsFromJump(const CheckersMove& currentMove, int verticalDirection,
		int horizontalDirection);

	// Will translate a move which is one space ahead.
	BoardIndex GetTranslatedMove(const BoardIndex& source, int verticalDirection, int horizontalDirection);

	// Will translate a jump which is two spaces ahead.
	BoardIndex GetTranslatedJump(const BoardIndex& source, int verticalDirection, int horizontalDirection);

	// Will return a string representation of a BaordIndex.
	std::string BoardIndexToString(const BoardIndex& index) const;

	// Returns PieceDisplayType for given index.
	PieceDisplayType GetPieceForIndex(const BoardIndex& index) const;

	// Returns the appropriate piece for the destination of the move that was given.
	PieceDisplayType GetPieceForMove(const CheckersMove& move) const;

	// Will construct and return a CheckersMove.
	CheckersMove CreateCheckersMove(const BoardIndex& sourceIndex,
		const BoardIndex& destinationIndex) const;

	// Contains every movable index and what type of piece if any is there.
	BoardData m_boardData;

	// Stores the source move and destination move when requested by the player.
	std::unique_ptr<CheckersMoveLauncher> m_pendingMoveLauncher;

	// This list is populated each turn and represents all possible moves of that player.
	std::vector<CheckersMove> m_legalDestinations;

	// If this list is not empty, a move must come from this list.
	std::vector<CheckersMove> m_legalJumpDestinations;

	// Toggle value. If it is not white player's turn, it is black players turn.
	bool m_isWhitePlayerTurn;
};

//---------------------------------------------------------------

struct CheckersMove
{
	CheckersMove(BoardIndex source, BoardIndex destination, int verticalDirection = 0,
		int horizontalDirection = 0);

	BoardIndex m_moveSource;
	BoardIndex m_moveDestination;

	// For simplicity, we store the direction of the move to easily be able to clear pieces
	// and look for further jumps. We could figure this out but why not just store it.
	int m_verticalDirection;
	int m_horizontalDirection;
	int m_rowDistance;
	int m_columnDistance;
	int m_moveLength;
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
	const CheckersMove& GetCheckersMove() const;

	bool IsSourceSet() const;

private:
	void SetMoveDirection();
	void SetMoveLength();
	void SetMoveDistance();

	CheckersMove m_checkersMove;

	bool m_isSourceSet;
	bool m_isDestinationSet;

	// This is called when both source and destination moves are set.
	std::function<void()> m_launchMoveCallback;
};
