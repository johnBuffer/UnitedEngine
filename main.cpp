#include <SFML/Graphics.hpp>

#include "united_solver.h"
#include "DisplayManager.h"
#include <sstream>
#include "props.h"
#include <stdlib.h>
#include "utils.hpp"

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(false);
	//window.setFramerateLimit(60);

	float body_radius = 25;
	up::Vec2 world_dimension(25000.0f, 50000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 900.0f });

	DisplayManager displayManager(&window, &solver);
	displayManager.setZoom(0.5);

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::White);

	while (window.isOpen())
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		displayManager.processEvents();

		solver.update(0.016f);

		window.clear(sf::Color::White);

		displayManager.draw(false);

		sf::RectangleShape rec(sf::Vector2f(400, 150));
		rec.setPosition(10, 10);
		rec.setFillColor(sf::Color::Black);
		window.draw(rec);

		/*text.setString("Objects: " + to_string(current_bodies));
		text.setPosition(20, 20);
		window.draw(text);
		*/
		text.setString("Physics time : " + round(solver.physicsUpdateTime(), 2) + " ms");
		text.setPosition(20, 45);
		window.draw(text);

		text.setString("Render time : " + round(displayManager.render_time, 2) + " ms");
		text.setPosition(20, 70);
		window.draw(text);

		float total_time = displayManager.render_time + solver.physicsUpdateTime();
		text.setString("Total frame time : " + round(total_time, 2) + " ms");
		text.setPosition(20, 95);
		window.draw(text);

		text.setString("FPS : " + round(1000.0f / total_time, 1));
		text.setPosition(20, 120);
		window.draw(text);

		window.display();

		
	}

	return 0;
}