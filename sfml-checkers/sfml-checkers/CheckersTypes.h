//---------------------------------------------------------------
//
// CheckersTypes.h
//

#pragma once
#include <vector>

namespace {
	const float s_squareSize = 100.0f;
	const int s_boardSize = 8;
	const float s_pieceSize = 25.0f;
}

enum PieceDisplayType
{
	BLACK,
	WHITE,
	BLACK_KING,
	WHITE_KING,
	EMPTY,
	INVALID = -1,
};

struct Position
{
	Position() = default;
	~Position() = default;
	Position(int32_t row_, int32_t col_)
		: row(row_)
		, col(col_)
	{
	}
	Position(const Position& rhs)
	{
		row = rhs.row;
		col = rhs.col;
	}
	bool operator==(const Position& rhs) const { return rhs.row == row && rhs.col == col; };
	bool operator!=(const Position& rhs) const { return !(*this == rhs); }
	int32_t row = -1;
	int32_t col = -1;
};

typedef std::vector<std::vector<PieceDisplayType>> BoardData;
