//---------------------------------------------------------------
//
// CheckersTypes.h
//

#pragma once

namespace {
	const float s_squareSize = 100.0f;
	const int s_boardSize = 8;
}

enum SpaceStatus
{
	BLACK,
	WHITE,
	BLACK_KING,
	WHITE_KING,
	EMPTY,
};
