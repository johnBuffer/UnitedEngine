#include <SFML/Graphics.hpp>

#include "united_solver.h"
#include "DisplayManager.h"

#include "props.h"

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	up::Vec2 world_dimension(5000.0f, 10000.0f);
	up::UnitedSolver solver(world_dimension, 15, { 0.0f, 200.0f });

	DisplayManager displayManager(&window, &solver);
	displayManager.setOffset(-world_dimension.x / 2, -0.75*world_dimension.y);
	displayManager.setZoom(0.5);

	uint32_t n = 32000;
	uint32_t i = 0;

	bool emit = false;

	while (window.isOpen())
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		displayManager.processEvents();
			

		if (i < n && emit)
		{
			i += 10;
			for (int ii(10); ii--;)
				solver.addBody({ float(rand() % 1000 + 1500), float(rand() % 100 + 9900) });
		}

		solver.update(0.016f);

		window.clear(sf::Color::White);

		displayManager.draw(false);

		window.display();
	}

	return 0;
}