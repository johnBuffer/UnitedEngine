#include <SFML/Graphics.hpp>

#include "united_solver.hpp"
#include "display_manager.hpp"
#include <sstream>
#include <stdlib.h>
#include "utils.hpp"
#include "segment.hpp"

#include <iostream>

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	const float body_radius(25.0f);
	up::Vec2 world_dimension(2000.0f, 2000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 2000.0f });

	DisplayManager displayManager(window, solver);
	displayManager.setZoom(1.0f);

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::White);

	while (window.isOpen())
	{
		displayManager.processEvents();

		if (displayManager.emit)
			solver.addBody({ 50.0f + rand()%2, 50.0f });

		solver.update(0.016f);

		window.clear(sf::Color::White);
		displayManager.draw(false);
		window.display();
	}

	return 0;
}