#include <SFML/Graphics.hpp>

#include "united_solver.h"
#include "DisplayManager.h"

#include "props.h"

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	float body_radius = 25;
	up::Vec2 world_dimension(10000.0f, 20000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 1000.0f });

	DisplayManager displayManager(&window, &solver);
	//displayManager.setOffset(-world_dimension.x / 2, -0.75*world_dimension.y);
	displayManager.setZoom(0.5);

	uint32_t n = 50000;

	while (window.isOpen())
	{
		uint32_t current_bodies = solver.bodies().size();
		if (current_bodies < n)
		{
			for (int i(10); --i;)
			{
				up::BodyPtr b = solver.addBody(up::Vec2(rand()%200, 3000 + rand() % 1000));
				b->setVelocity({ rand() % 10 + 50.0f, rand()%10+1.0f });
			}

			/*for (int i(1000); --i;)
			{
				up::BodyPtr b = solver.addBody(up::Vec2(rand() % 10000+0.0f, rand() % 12000 + 8000.0f));
			}*/
		}

		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		displayManager.processEvents();

		solver.update(0.016f);

		window.clear(sf::Color::White);

		displayManager.draw(false);

		window.display();
	}

	return 0;
}