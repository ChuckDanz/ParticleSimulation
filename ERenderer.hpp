#ifndef ERENDERER_HPP
#define ERENDERER_HPP

#include <SFML/Graphics.hpp>
#include <optional>
#include "Esolver.hpp"

struct ERenderer
{
	sf::Image                image;
	sf::Texture              texture;
	std::optional<sf::Sprite> sprite;

	ERenderer(int W, int H)
		: image(sf::Vector2u{(unsigned)W, (unsigned)H}, sf::Color::Black)
	{
		[[maybe_unused]] bool ok = texture.resize(sf::Vector2u{(unsigned)W, (unsigned)H});
		sprite.emplace(texture);
	}

	sf::Color cellColor(const GridCell& cell)
	{
		if (cell.conductive)
			return sf::Color(220, 235, 255); // bright blue-white discharge channel

		if (cell.isFixed)
			return sf::Color(20, 20, 30); // ground row — near-black

		float p = cell.potential;
		uint8_t r = static_cast<uint8_t>(p * p * 50.0f);
		uint8_t g = static_cast<uint8_t>(p * p * 80.0f);
		uint8_t b = static_cast<uint8_t>(p * 180.0f + 5.0f);
		return sf::Color(r, g, b);
	}

	void render(sf::RenderWindow& window, const ESolver& solver)
	{
		for (int r = 0; r < solver.H; r++)
			for (int c = 0; c < solver.W; c++)
				image.setPixel({(unsigned)c, (unsigned)r}, cellColor(solver.grid[r][c]));

		texture.update(image);
		window.draw(*sprite);
	}
};

#endif // ERENDERER_HPP
