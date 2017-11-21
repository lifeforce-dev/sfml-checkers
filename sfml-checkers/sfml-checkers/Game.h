//---------------------------------------------------------------
//
// Game.h
//

#pragma once

#include "CheckersTypes.h"

#include <memory>
#include <functional>

struct Vector2D
{
	Vector2D() {}
	Vector2D(int32_t y, int32_t x)
		: y(y)
		, x(x)
	{
	}

	int32_t x;
	int32_t y;
};

struct Range
{
	Range(int32_t min_, int32_t max_)
		: min(min_)
		, max(max_) {}
	int32_t min = 0;
	int32_t max = 0;
	bool Contains(int32_t x) const { return x >= min && x <= max; }
};

//---------------------------------------------------------------

class CheckersMoveHelper;
class CheckersMove;
class Game
{
	friend class CheckersMoveHelper;
public:
	Game();
	~Game();

	const BoardData& GetBoardData() { return m_boardData; }

	int GetBoardSize();

	// Return the correct board index for UI coordinates.
	Position GetPositionFromRowCol(int row, int col) const;

	// Called in response to the UI reporting that a player made a selection.
	void OnMoveSelectionEvent(const Position& position);

private:
	// This is called from the CheckersMoveLauncher when the move destination is selected.
	// This will reset the CheckersMoveLauncher unconditionally, and attempt to move.
	void OnLaunchMove(const CheckersMove& move);

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
	bool IsValidPosition(const Position& position) const;

	// Returns whether the move is available in the legal move list.
	bool IsLegalMove(const CheckersMove& move) const;

	// Returns whether the move is available in the legal jump list.
	bool IsLegalJump(const CheckersMove& move) const;

	// Returns whether the move results in a kingable position.
	bool IsKingableIndex(const Position& move) const;

	// Returns whether the specified index contains a piece.
	bool ContainsPiece(const Position& position) const;

	// Returns whether the specified index contains a piece of the current player's turn.
	bool ContainsPlayerPiece(const Position& position) const;

	// Returns whether the specified index contains a piece of the enemy of the current player's turn.
	bool ContainsEnemyPiece(const Position& position) const;

	// Returns whether the specified piece belongs to the player whose turn it currently is.
	bool IsPieceOfCurrentPlayer(PieceDisplayType piece) const;

	// This will attempt to add possible moves for a specific piece.
	void EvaluatePossibleMovesForIndex(const Position& pieceIndex);

	// Given a direction, will add a move to the appropriate list if it is valid.
	void AddValidMoveForDirection(const Position& currentPosition, int verticalDirection,
		int horizontalDirection);

	// Given a direction, will add a valid jump to the jump list.
	void AddValidJumpForDirection(const Position& currentPosition, int verticalDirection,
		int horizontalDirection);

	// Given a jump, will try and assemble the next jump in the chain if one exists.
	void AddValidJumpsFromJump(const CheckersMove& currentMove, int verticalDirection,
		int horizontalDirection);

	// Will translate a move which is one space ahead.
	Position GetTranslatedMove(const Position& source, int verticalDirection, int horizontalDirection);

	// Will translate a jump which is two spaces ahead.
	Position GetTranslatedJump(const Position& source, int verticalDirection, int horizontalDirection);

	// Will return a string representation of a BaordIndex.
	std::string PositionToString(const Position& index) const;

	// Returns PieceDisplayType for given index.
	PieceDisplayType GetPieceForIndex(const Position& index) const;

	// Returns the appropriate piece for the destination of the move that was given.
	PieceDisplayType GetPieceForMove(const CheckersMove& move) const;

	// Will construct and return a CheckersMove.
	CheckersMove CreateCheckersMove(const Position& sourceIndex,
		const Position& destinationIndex) const;

	// Contains every movable index and what type of piece if any is there.
	BoardData m_boardData;

	// Helper object that moves a piece from source to dest.
	std::unique_ptr<CheckersMoveHelper> m_moveHelper;

	// This list is populated each turn and represents all possible moves of that player.
	std::vector<CheckersMove> m_legalDestinations;

	// If this list is not empty, a move must come from this list.
	std::vector<CheckersMove> m_legalJumpDestinations;

	// Toggle value. If it is not white player's turn, it is black players turn.
	bool m_isWhitePlayerTurn;
};

//---------------------------------------------------------------
class CheckersMove
{
public:
	CheckersMove() {}
	CheckersMove(Position source, Position destination);
	void SetSource(Position source) { m_source = source; }
	void SetDestination(Position destination);

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

	void UpdateMoveDistance();
	void UpdateMoveLength();
	void UpdateMoveDirection();
};

//---------------------------------------------------------------

class CheckersMoveHelper
{
public:
	CheckersMoveHelper(Game* game);
	~CheckersMoveHelper();

	// This will through sequential calls, set the source and destination.
	void HandleMoveSelected(const Position& moveSource);

	const CheckersMove& GetCheckersMove() const;

private:
	Game* m_game;
	CheckersMove m_checkersMove;

	// This is called when both source and destination moves are set.
	std::function<void()> m_launchMoveCallback;

	bool m_isSelectingMove = false;
};
