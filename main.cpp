#include <SFML/Graphics.hpp>

#include "united_solver.h"
#include "DisplayManager.h"
#include <sstream>
#include "props.h"

std::string to_string(uint32_t number)
{
	std::stringstream ss;
	ss << number;

	return ss.str();
}

std::string round(double d, int decimals)
{
	std::string result;
	std::stringstream sx;
	sx << std::fixed << d;
	result = sx.str();

	size_t pos = result.find('.');
	if (pos == std::string::npos)
		return result;
	else if (!decimals)
	{
		return result.substr(0, pos);
	}
	else
	{
		if (pos + decimals + 1 < result.size())
		{
			return result.substr(0, pos + decimals + 1);
		}
	}

	return result;
}

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(60);

	float body_radius = 25;
	up::Vec2 world_dimension(25000.0f, 50000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 900.0f });

	DisplayManager displayManager(&window, &solver);
	displayManager.setZoom(0.5);

	uint32_t n = 100000;

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::White);

	while (window.isOpen())
	{
		uint32_t current_bodies = solver.bodies().size();
		if (current_bodies < n && displayManager.emit)
		{
			for (int i(10); --i;)
			{
				up::BodyPtr b = solver.addBody(up::Vec2(body_radius, 35000.0f + i * 2 * body_radius));
				b->setVelocity({ 70.0f, 0.0f});
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

		sf::RectangleShape rec(sf::Vector2f(400, 150));
		rec.setPosition(10, 10);
		rec.setFillColor(sf::Color::Black);
		window.draw(rec);

		text.setString("Objects: " + to_string(current_bodies));
		text.setPosition(20, 20);
		window.draw(text);

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