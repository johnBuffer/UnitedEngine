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

	float body_radius = 10;
	up::Vec2 world_dimension(10000.0f, 10000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 200.0f });

	DisplayManager displayManager(&window, &solver);
	displayManager.setOffset(-world_dimension.x / 2, -0.75*world_dimension.y);
	displayManager.setZoom(0.5);

	uint32_t n = 32000;

	Wall wall(up::Vec2(5000, 10000-body_radius*201), 4, 100);
	wall.init(solver);

	up::BodyPtr b = solver.addBody(up::Vec2(2000, 8500));

	while (window.isOpen())
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		displayManager.processEvents();

		b->moveHard({ 4*body_radius, 0.0f });
		b->stop();

		solver.update(0.016f);

		window.clear(sf::Color::White);

		displayManager.draw(false);
		wall.draw(&window, displayManager);

		window.display();
	}

	return 0;
}