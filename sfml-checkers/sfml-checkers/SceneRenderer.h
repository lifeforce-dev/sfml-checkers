//---------------------------------------------------------------
//
// SceneRenderer.h
//

#pragma once

#include "CheckersTypes.h"

#include <SFML/Graphics.hpp>

class Game;
struct CheckersSquare;

class SceneRenderer
{
public:
	SceneRenderer(sf::RenderTarget& target, Game* game);
	~SceneRenderer();

	void Draw();
	void OnMouseClick(sf::Vector2i localPosition);

private:
	void BuildBoardBackground();
	void DrawBoardBackground();
	void DrawBoardPieces();

	void ApplyPieceColor(PieceDisplayType pieceDisplayType, sf::CircleShape& piece);

	sf::RenderTarget* m_renderTarget;
	std::vector<CheckersSquare> m_checkersSquares;
	Game* m_game;
};

struct CheckersSquare
{
	CheckersSquare(const sf::RectangleShape square, const BoardIndex& boardIndex);

	// The drawable representation of the CheckersSquare.
	sf::RectangleShape m_square;

	// Where on the board is this square in row, col.
	BoardIndex m_boardIndex;
};
