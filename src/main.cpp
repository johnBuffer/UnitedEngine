#include <SFML/Graphics.hpp>

#include "united_solver.hpp"
#include "display_manager.hpp"
#include <sstream>
#include <stdlib.h>
#include "utils.hpp"
#include "segment.hpp"

#include <iostream>

void addBox(up::UnitedSolver& solver, float x, float y, float w, float h)
{
	const float radius(12.0f);
	up::BodyPtr b1 = solver.addBody(up::Vec2(x, y), radius);
	up::BodyPtr b2 = solver.addBody(up::Vec2(x + w, y), radius);
	up::BodyPtr b3 = solver.addBody(up::Vec2(x + w, y + h), radius);
	up::BodyPtr b4 = solver.addBody(up::Vec2(x, y + h), radius);

	up::SolidSegmentPtr segment1 = solver.addSolidSegment(b1, b2);
	up::SolidSegmentPtr segment2 = solver.addSolidSegment(b2, b3);
	up::SolidSegmentPtr segment3 = solver.addSolidSegment(b3, b4);
	up::SolidSegmentPtr segment4 = solver.addSolidSegment(b4, b1);

	solver.addConstraint(b1, b3);
	solver.addConstraint(b2, b4);
}

void addSolidSegment(up::UnitedSolver& solver, float x1, float y1, float x2, float y2, bool moving = true)
{
	const float radius(8.0f);
	up::BodyPtr b1 = solver.addBody(up::Vec2(x1, y1), radius);
	up::BodyPtr b2 = solver.addBody(up::Vec2(x2, y2), radius);

	b1->moving(moving);
	b2->moving(moving);

	up::SolidSegmentPtr segment1 = solver.addSolidSegment(b1, b2);
}

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	const float body_radius(16.0f);
	up::Vec2 world_dimension(16000.0f, 8000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 900.0f });

	DisplayManager displayManager(window, solver);
	displayManager.setZoom(0.5f);
	displayManager.setOffset(0.65f * world_dimension);

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::White);

	sf::Clock clock;

	//addSolidSegment(solver, 500, 1000, 1000, 1200, false);
	//addSolidSegment(solver, 500, 100, 700, 100);

	uint32_t bodies(0);

	while (window.isOpen())
	{
		const sf::Vector2i mouse_pos(sf::Mouse::getPosition(window));
		const up::Vec2 world_coord(displayManager.displayCoordToWorldCoord(up::Vec2(mouse_pos.x, mouse_pos.y)));

		if (displayManager.emit && bodies < 64000) {
			uint32_t nb(10);
			for (uint8_t i(nb); i--;) {
				up::BodyPtr b= solver.addBody(up::Vec2(2.0f*body_radius, 2000 + i*2*body_radius));
				b->setVelocity(up::Vec2(4*body_radius, 0));
			}

			bodies += nb;
		}

		displayManager.processEvents();

		if (displayManager.clic) {
			displayManager.clic = false;
			//addSolidSegment(solver, world_coord.x, world_coord.y, world_coord.x, world_coord.y + 100.0f);
			addBox(solver, world_coord.x, world_coord.y, 50.0f + rand()%200, 50.0f + rand() % 200);
			//addBox(solver, world_coord.x, world_coord.y, 100.0f, 100.0f);
		}

		if (1 || displayManager.emit) {
			solver.update(0.016f);
		}

		window.clear(sf::Color::White);
		
		displayManager.draw(false);

		sf::RectangleShape rec(sf::Vector2f(400, 150));
		rec.setPosition(10, 10);
		rec.setFillColor(sf::Color::Black);
		window.draw(rec);

		text.setString("Objects: " + to_string(bodies));
		text.setPosition(20, 20);
		window.draw(text);

		text.setString("Grid time : " + round(solver.getGridTime(), 2) + " ms");
		text.setPosition(20, 45);
		window.draw(text);

		text.setString("Collision time : " + round(solver.getCollisionTime(), 2) + " ms");
		text.setPosition(20, 70);
		window.draw(text);
		
		window.display();
	}

	return 0;
}