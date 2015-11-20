//---------------------------------------------------------------
//
// SceneRenderer.h
//

#pragma once

#include "CheckersTypes.h"

#include <SFML/Graphics.hpp>

class Game;

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
	std::vector<sf::RectangleShape> m_boardBackground;
	Game* m_game;
};
