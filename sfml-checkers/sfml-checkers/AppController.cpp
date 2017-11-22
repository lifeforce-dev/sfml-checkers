//---------------------------------------------------------------
//
// AppController.cpp
//

#include "AppController.h"
#include <iostream>

namespace Checkers {

//========================================================================

AppController::AppController()
	: m_mainWindow(sf::VideoMode(800, 800), "Checkers")
	, m_sceneRenderer(m_mainWindow, &m_game)
	, m_game()
{
}

void AppController::Run()
{
	while (m_mainWindow.isOpen())
	{
		ProcessEvents();
		Draw();
	}
}

void AppController::Draw()
{
	if (m_mainWindow.isOpen())
	{
		m_sceneRenderer.Draw();
		m_mainWindow.display();
	}
}

void AppController::ProcessEvents()
{
	sf::Event event;
	while (m_mainWindow.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_mainWindow.close();

		if (event.type == sf::Event::MouseButtonPressed)
		{
			m_sceneRenderer.OnMouseClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
		}
	}
}

//========================================================================

} // namespace Checkers
