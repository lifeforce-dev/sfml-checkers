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

private:
	void BuildBoardBackground();
	void DrawBoardBackground();

	sf::RenderTarget* m_renderTarget;
	std::vector<sf::RectangleShape> m_boardBackground;
	Game* m_game;
};
