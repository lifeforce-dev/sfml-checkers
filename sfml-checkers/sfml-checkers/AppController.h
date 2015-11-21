//---------------------------------------------------------------
//
// AppController.h
//

#pragma once

#include "SceneRenderer.h"
#include "Game.h"

#include <SFML/Graphics.hpp>

class AppController
{
public:
	AppController();

	void Run();
	void Draw();
	void ProcessEvents();

private:
	sf::RenderWindow m_mainWindow;
	Game m_game;
	SceneRenderer m_sceneRenderer;
};
