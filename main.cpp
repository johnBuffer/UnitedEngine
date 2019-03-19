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
	window.setVerticalSyncEnabled(true);
	//window.setFramerateLimit(60);

	float body_radius = 12;
	up::Vec2 world_dimension(2000.0f, 2000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 900.0f });

	DisplayManager displayManager(&window, &solver);
	displayManager.setZoom(0.75);

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::White);

	up::BodyPtr b1 = solver.addBody(50, 50);
	up::BodyPtr b2 = solver.addBody(100, 50);
	up::BodyPtr b3 = solver.addBody(100, 100);
	up::BodyPtr b4 = solver.addBody(50, 100);

	solver.addConstraint(b1, b2);
	solver.addConstraint(b2, b3);
	solver.addConstraint(b3, b4);
	solver.addConstraint(b4, b1);
	solver.addConstraint(b3, b1);
	solver.addConstraint(b4, b2);
	

	up::Vec2 pt1(0, 0);

	for (int i(0); i < 0; i++)
	{
		up::Vec2 sp1(rand() % int(world_dimension.x), rand() % int(world_dimension.y));
		up::Vec2 sp2(rand() % int(world_dimension.x), rand() % int(world_dimension.y));

		solver.addSegment(sp1, sp2);
	}

	while (window.isOpen())
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		displayManager.processEvents();

		up::Vec2 pt2(mousePosition.x, mousePosition.y);
		sf::VertexArray segment(sf::Lines, 2);
		up::Vec2 p1 = displayManager.worldCoordToDisplayCoord(pt1);
		segment[0].position = sf::Vector2f(p1.x, p1.y);
		segment[1].position = sf::Vector2f(pt2.x, pt2.y);

		solver.update(0.016f);

		auto intersections = solver.getIntersectionWith(pt1, displayManager.displayCoordToWorldCoord(pt2));

		window.clear(sf::Color::White);

		displayManager.draw(false);
		window.draw(segment);

		for (const up::Vec2& inter_pt : intersections)
		{
			sf::CircleShape cs(4.0f);
			cs.setOrigin(4.0f, 4.0f);
			cs.setFillColor(sf::Color::Green);

			const up::Vec2 dc_inter = displayManager.worldCoordToDisplayCoord(inter_pt);

			cs.setPosition(dc_inter.x, dc_inter.y);

			window.draw(cs);
		}

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