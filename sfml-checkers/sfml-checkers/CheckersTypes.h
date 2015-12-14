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

typedef std::vector<std::vector<PieceDisplayType>> BoardData;
typedef std::pair<int, int> BoardIndex;
