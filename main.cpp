#include <SFML/Graphics.hpp>

#include "united_solver.h"

int main()
{
	const uint32_t win_height = 800;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(1000, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	UnitedSolver solver({ 10000.0f, 10000.0f }, { 0.0f, 980.0f });

	uint32_t n = 8000;
	uint32_t i = 0;
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::A)
				{
					solver.test_pressure = !solver.test_pressure;
				}
			}
		}

		if (i < n)
		{
			i += 5;
			solver.addBody({ {float(rand() % 1000), float(rand() % 200)}, 8.0f, 1.0f });
			solver.addBody({ {float(rand() % 1000), float(rand() % 200)}, 8.0f, 1.0f });
			solver.addBody({ {float(rand() % 1000), float(rand() % 200)}, 8.0f, 1.0f });
			solver.addBody({ {float(rand() % 1000), float(rand() % 200)}, 8.0f, 1.0f });
			solver.addBody({ {float(rand() % 1000), float(rand() % 200)}, 8.0f, 1.0f });
		}

		solver.update(0.016f);

		window.clear();

		const std::vector<Body>& bodies = solver.bodies();
		for (const Body& b : bodies)
		{
			float radius = b.radius();
			sf::CircleShape c(radius);
			c.setOrigin(radius, radius);
			c.setFillColor(sf::Color::Green);
			c.setPosition(b.position().x, b.position().y);
			window.draw(c);
		}

		window.display();
	}

	return 0;
}