#include <SFML/Graphics.hpp>

#include "united_solver.h"

int main()
{
	const uint32_t win_height = 800;
	sf::RenderWindow window(sf::VideoMode(1000, win_height), "UE2");
	window.setVerticalSyncEnabled(false);

	UnitedSolver solver({ 1000.0f, float(win_height) }, { 0.0f, 9.8f/0.016f });

	uint32_t n = 500;
	for (int i(n); i--;)
	{
		solver.addBody({ {float(rand() % 1000), float(rand() % 200)}, 12.0f, 1.0f });
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
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