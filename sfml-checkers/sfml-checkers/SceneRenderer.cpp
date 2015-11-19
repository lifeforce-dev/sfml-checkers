//---------------------------------------------------------------
//
// SceneRenderer.cpp
//

#include "SceneRenderer.h"

SceneRenderer::SceneRenderer(sf::RenderTarget& target) : m_renderTarget(&target)
{
	BuildBoardBackground();
}

SceneRenderer::~SceneRenderer()
{
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

void SceneRenderer::Draw()
{
	m_renderTarget->clear();

	DrawBoardBackground();
}
