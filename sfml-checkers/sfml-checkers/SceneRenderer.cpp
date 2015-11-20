//---------------------------------------------------------------
//
// SceneRenderer.cpp
//

#include "SceneRenderer.h"
#include "Game.h"

#include <assert.h>

SceneRenderer::SceneRenderer(sf::RenderTarget& target, Game* game)
	: m_renderTarget(&target)
	, m_game(game)
{
	BuildBoardBackground();
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::Draw()
{
	m_renderTarget->clear();

	DrawBoardBackground();
	DrawBoardPieces();
}

void SceneRenderer::OnMouseClick(sf::Vector2i localPosition)
{
	// TODO:
	// On setup.
	// Store all of the clickable spaces and associate with an array index

	// On click.
	// If this space has a piece.
		// Select it
		// Populate that pieces possible moves.
	// If a space is selected
		// <in game logic, not in UI>
		// If new space is a valid move.
			// Move piece.
			// Deselect all
}

void SceneRenderer::BuildBoardBackground()
{
	std::vector<sf::Color> colorPalette(s_boardSize);

	bool colorToggle = true;
	std::generate_n(colorPalette.begin(), s_boardSize, [colorToggle] () mutable {

		// Alternate colors.
		if (colorToggle = !colorToggle)
			return sf::Color(209, 139, 71, 255);
		else
			return sf::Color(255, 228, 170, 255);
	});

	for (int row = 0; row < s_boardSize; ++row)
	{
		for (int col = 0; col < s_boardSize; ++col)
		{
			sf::RectangleShape currentSquare = sf::RectangleShape(sf::Vector2f(s_squareSize, s_squareSize));
			currentSquare.setPosition(row * s_squareSize, col * s_squareSize);
			currentSquare.setFillColor(colorPalette[col]);

			m_boardBackground.push_back(currentSquare);
		}

		// Reverse the color palette to make the checker effect.
		std::reverse(colorPalette.begin(), colorPalette.end());
	}
}

void SceneRenderer::DrawBoardBackground()
{
	for (auto it = m_boardBackground.begin(); it != m_boardBackground.end(); ++it)
	{
		m_renderTarget->draw(*it);
	}
}

void SceneRenderer::DrawBoardPieces()
{
	const BoardData& boardData = m_game->GetBoardData();
	float yPieceSpacing = 75;
	float xPieceSpacing = 75;
	float xBorderPadding = 25;
	float yBorderPadding = 25;

	for (auto row = boardData.begin(); row != boardData.end(); ++row)
	{
		int rowNumber = std::distance(boardData.begin(), row);
		for (auto col = row->begin(); col != row->end(); ++col)
		{
			// This is a super lightweight object containing only data needed for OpenGL calls.
			// Its fine to construct and destroy in this loop. I'm also okay with creating one for
			// empty spaces, as it simplifies this code quite a bit.
			sf::CircleShape piece(s_pieceSize);

			int columnNumber = std::distance(row->begin(), col);
			piece.setPosition(xBorderPadding + ((s_pieceSize + xPieceSpacing) * columnNumber),
				yBorderPadding + ((s_pieceSize + yPieceSpacing) * rowNumber));

			ApplyPieceColor(*col, piece);

			m_renderTarget->draw(piece);
		}
	}
}

void SceneRenderer::ApplyPieceColor(PieceDisplayType pieceDisplayType, sf::CircleShape& piece)
{
	switch (pieceDisplayType)
	{
	case BLACK:
		piece.setFillColor(sf::Color(90, 53, 44, 255));
		break;
	case WHITE:
		piece.setFillColor(sf::Color(246, 221, 190, 255));
		break;
	case BLACK_KING:
		// TODO
		break;
	case WHITE_KING:
		// TODO
		break;
	case EMPTY:
		// Do not draw blank spaces.
		piece.setFillColor(sf::Color(0, 0, 0, 0));
		break;
	default:
		// This should never be the case. If we do have an unknown piece type, I am enforcing that
		// it is added to this switch.
		assert(0);
	}
}
