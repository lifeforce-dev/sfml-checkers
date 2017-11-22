//---------------------------------------------------------------
//
// CheckersTypes.h
//

#pragma once
#include <vector>

namespace Checkers {

//========================================================================

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

using BoardData = std::vector<std::vector<PieceDisplayType>>;

//========================================================================

} // namespace Checkers
