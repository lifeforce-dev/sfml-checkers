#include <SFML/Graphics.hpp>
#include <memory>

namespace {
	const float s_squareSize = 100.0f;
	const int s_boardSize = 8;
}

class SceneRenderer
{
public:
	SceneRenderer(sf::RenderTarget& target)
		: m_renderTarget(&target)
	{
		BuildBoardBackground();
	}

	void BuildBoardBackground()
	{
		std::vector<sf::Color> colorPalette;

		// Build color palette.
		for (int i = 0; i < s_boardSize; ++i)
		{
			sf::Color checkerColor;
			if (i % 2)
				checkerColor = sf::Color(209, 139, 71, 255);
			else
				checkerColor = sf::Color(255, 228, 170, 255);

			colorPalette.push_back(checkerColor);
		}

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
			if (row % 2)
				std::reverse(colorPalette.begin(), colorPalette.end());
			else
				std::reverse(colorPalette.begin(), colorPalette.end());
		}
	}

	void RenderBoardBackground()
	{
		for (auto it = m_boardBackground.begin(); it != m_boardBackground.end(); ++it)
		{
			m_renderTarget->draw(*it);
		}
	}

	void Render()
	{
		m_renderTarget->clear();

		RenderBoardBackground();
	}

	sf::RenderTarget* m_renderTarget;
	std::vector<sf::RectangleShape> m_boardBackground;
};

class AppController
{
public:
	AppController()
		: m_mainWindow(sf::VideoMode(800, 800), "Checkers")
		, m_sceneRenderer(m_mainWindow)
	{
	}

	void Run()
	{
		while (m_mainWindow.isOpen())
		{
			ProcessEvents();
			RenderGame();
		}
	}

	void RenderGame()
	{
		if (m_mainWindow.isOpen())
		{
			m_sceneRenderer.Render();
			m_mainWindow.display();
		}
	}

	void ProcessEvents()
	{
		sf::Event event;
		while (m_mainWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_mainWindow.close();
		}
	}

private:
	sf::RenderWindow m_mainWindow;
	SceneRenderer m_sceneRenderer;
};

int main()
{
	AppController appController;

	appController.Run();
	return 0;
}
