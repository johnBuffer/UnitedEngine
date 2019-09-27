#include <SFML/Graphics.hpp>

#include "united_solver.hpp"
#include "display_manager.hpp"
#include <sstream>
#include <stdlib.h>
#include "utils.hpp"
#include "segment.hpp"
#include <dynamic_blur.hpp>
#include <iostream>
#include <transition.hpp>


void addBox(up::UnitedSolver& solver, float x, float y, float w, float h)
{
	const float radius(20.0f);
	up::BodyPtr b1 = solver.addBody(up::Vec2(x    , y    ), radius);
	up::BodyPtr b2 = solver.addBody(up::Vec2(x + w, y    ), radius);
	up::BodyPtr b3 = solver.addBody(up::Vec2(x + w, y + h), radius);
	up::BodyPtr b4 = solver.addBody(up::Vec2(x    , y + h), radius);

	up::SolidSegmentPtr segment1 = solver.addSolidSegment(b1, b2);
	up::SolidSegmentPtr segment2 = solver.addSolidSegment(b2, b3);
	up::SolidSegmentPtr segment3 = solver.addSolidSegment(b3, b4);
	up::SolidSegmentPtr segment4 = solver.addSolidSegment(b4, b1);

	solver.addConstraint(b1, b3);
	solver.addConstraint(b2, b4);
}

void addSolidSegment(up::UnitedSolver& solver, float x1, float y1, float x2, float y2, bool moving = true)
{
	const float radius(20.0f);
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
	//window.setVerticalSyncEnabled(true);
	//window.setFramerateLimit(60);

	const float body_radius(16.0f);
	up::Vec2 world_dimension(12500.0f, 12500.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 980.0f });

	sf::RenderTexture render_tex;
	render_tex.create(win_width, win_height);

	DisplayManager displayManager(render_tex, window, solver);

	/*up::BodyPtr b = solver.addBody(up::Vec2(50, 50));
	b->moving(false);*/

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color(250, 250, 250));

	sf::Clock clock, spawn_timer;
	Blur blur(win_width, win_height, 0.5f);

	//addSolidSegment(solver, 20, 100, 1100, 900, false);

	trn::Transition<float> zoom(1.0f);
	//zoom.setSpeed(3.0f);
	trn::Transition<up::Vec2> offset(up::Vec2(200.0f, 15800));
	//offset.setSpeed(3.0f);

	bool double_flow(false);
	uint32_t nb(1);
	float delay(0.5f);
	bool next(false);
	uint32_t bodies(0);

	while (window.isOpen())
	{
		const sf::Vector2i mouse_pos(sf::Mouse::getPosition(window));
		const up::Vec2 world_coord(displayManager.displayCoordToWorldCoord(up::Vec2(mouse_pos.x, mouse_pos.y)));

		/*if (displayManager.debug_mode)
			b->setPosition(world_coord);*/

		clock.restart();

		uint32_t n(100);

		if (displayManager.emit && bodies < 120000) {
			for (int i(0); i < n; ++i) {
				solver.addBody(up::Vec2(rand() % int32_t(world_dimension.x), rand() % 200));
				/*auto b = solver.addBody(up::Vec2(7000 + body_radius * 2 * i, 12800 - body_radius));
				b->setVelocity(up::Vec2(5 - rand() % 10, -100.0f));*/
				/*auto b = solver.addBody(up::Vec2(5000+ 2*i*body_radius, body_radius));
				b->setVelocity(up::Vec2((10 - rand()%20)*0.1f, 40.0f));*/
			}
			
			bodies += n;
		}

		displayManager.processEvents();

		if (displayManager.clic) {
			up::Vec2 clic_pos_display_coord = displayManager.getClicPosition();
			up::Vec2 clic_pos_world_coord = displayManager.displayCoordToWorldCoord(displayManager.getClicPosition());
			/*solver.resetDebug();
			up::Body* b(solver.getBodyAt(displayManager.displayCoordToWorldCoord(clic_pos_display_coord)));

			if (b) {
				b->debug = true;
			}*/

			addBox(solver, clic_pos_world_coord.x, clic_pos_world_coord.y, 200, 200);
		}

		if (displayManager.clic) {
			displayManager.clic = false;
			//addSolidSegment(solver, world_coord.x, world_coord.y, world_coord.x, world_coord.y + 100.0f);
			//addBox(solver, world_coord.x, world_coord.y, 50.0f + rand()%200, 50.0f + rand() % 200);
			//addBox(solver, world_coord.x, world_coord.y, 100.0f, 100.0f);
		}

		if (displayManager.update) {
			solver.update(0.016f);
		}

		render_tex.clear(sf::Color::White);

		displayManager.draw(false);

		render_tex.display();
		window.draw(sf::Sprite(render_tex.getTexture()));
		blur.setRegion(10, 10, 400, 150);
		window.draw(blur.apply(render_tex.getTexture(), 3));
		sf::RectangleShape rec(sf::Vector2f(400, 150));
		rec.setPosition(10, 10);
		rec.setFillColor(sf::Color(50, 50, 50, 100));
		window.draw(rec);

		text.setString("Objects: " + to_string(bodies));
		text.setCharacterSize(36);
		text.setPosition(25, 15);
		window.draw(text);

		text.setCharacterSize(20);
		text.setString("Physics time: " + round(solver.getCollisionTime() + solver.getGridTime(), 2) + " ms");
		text.setPosition(20, 70);
		window.draw(text);

		text.setString("Render time: " + round(displayManager.render_time, 2) + " ms");
		text.setPosition(20, 95);
		window.draw(text);

		text.setString("Frame time: " + round(clock.getElapsedTime().asMilliseconds(), 2) + " ms");
		text.setPosition(20, 120);
		window.draw(text);

		window.display();
	}

	return 0;
}