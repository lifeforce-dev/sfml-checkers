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

struct BoardIndex
{
	BoardIndex() = default;
	~BoardIndex() = default;
	BoardIndex(int32_t row_, int32_t col_)
		: row(row_)
		, col(col_)
	{
	}
	BoardIndex(const BoardIndex& rhs)
	{
		row = rhs.row;
		col = rhs.col;
	}
	bool operator==(const BoardIndex& rhs) const { return rhs.row == row && rhs.col == col; };
	int32_t row = -1;
	int32_t col = -1;
};

typedef std::vector<std::vector<PieceDisplayType>> BoardData;
