#include <SFML/Graphics.hpp>

#include "united_solver.hpp"
#include "display_manager.hpp"
#include <sstream>
#include <stdlib.h>
#include "utils.hpp"
#include "segment.hpp"
#include "dynamic_blur.hpp"
#include <iostream>
#include <transition.hpp>


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
	window.setVerticalSyncEnabled(false);

	const float body_radius(16.0f);
	up::Vec2 world_dimension(16000.0f, 16000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 900.0f });

	sf::RenderTexture render_tex;
	render_tex.create(win_width, win_height);

	DisplayManager displayManager(render_tex, window, solver);

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color(250, 250, 250));

	sf::Clock clock, spawn_timer;
	Blur blur(win_width, win_height, 0.5f);

	//addSolidSegment(solver, 8000, 8000, 8000, 3500, false);
	//addSolidSegment(solver, 500, 100, 700, 100);


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

		clock.restart();

		if (displayManager.emit && bodies < 180000) {

			displayManager.setZoom(zoom);
			displayManager.setOffset(offset);

			if (spawn_timer.getElapsedTime().asSeconds() > delay) {
				spawn_timer.restart();
				for (uint8_t i(nb); i--;) {
					if (bodies < 10) {
						delay = 0.5f;
						solver.addBody(up::Vec2(2.0f*body_radius + rand() % 2, 15900));
					}
					else if (!next && bodies == 10) {
						next = true;
						delay = 0.25f;
						zoom = 0.9f;
						offset = up::Vec2(400.0f, 15800.0f);
					}
					else if (next || bodies < 50) {
						next = false;
						solver.addBody(up::Vec2(2.0f*body_radius + rand() % 500, 15500));
					}
					else if (!next && bodies == 50) {
						delay = 0.125f;
						zoom = 0.8f;
						offset = up::Vec2(500.0f, 15800.0f);
						next = true;
					}
					else if (next || bodies < 100) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 15500));
						b->setVelocity(up::Vec2(10.0f, 0.0f));
					}
					else if (!next && bodies == 100) {
						delay = 0.1f;
						nb = 2;
						zoom = 0.75f;
						offset = up::Vec2(600.0f, 15800.0f);
						next = true;
					}
					else if (next || bodies < 200) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 15500 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(10.0f, 0.0f));
					}
					else if (!next && bodies == 200) {
						delay = 0.1f;
						nb = 4;
						zoom = 0.6f;
						offset = up::Vec2(800.0f, 15500.0f);
						next = true;
					}
					else if (bodies < 400) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 15000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(10.0f, 0.0f));
					}
					else if (!next && bodies == 400) {
						delay = 0.05f;
						nb = 5;
						zoom = 0.4f;
						offset = up::Vec2(900.0f, 15200.0f);
						next = true;
					}
					else if (bodies < 1000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 14500 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(15.0f, 0.0f));
					}
					else if (!next && bodies == 1000) {
						delay = 0.05f;
						nb = 8;
						zoom = 0.3f;
						offset = up::Vec2(1000.0f, 14800.0f);
						next = true;
					}
					else if (bodies < 2000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 14000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(15.0f, 0.0f));
					}
					else if (!next && bodies == 2000) {
						delay = 0.05f;
						nb = 15;
						zoom = 0.3f;
						offset = up::Vec2(1600.0f, 14800.0f);
						next = true;
					}
					else if (bodies < 4000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 14000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(15.0f, 0.0f));
					}
					else if (!next && bodies == 4010) {
						delay = 0.04f;
						nb = 16;
						zoom = 0.2f;
						offset = up::Vec2(2000.0f, 14800.0f);
						next = true;
					}
					else if (bodies < 10000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 14000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(1.5f*body_radius, 0.0f));
					}
					else if (!next && bodies == 10010) {
						delay = 0.02f;
						nb = 17;
						zoom = 0.1f;
						offset = up::Vec2(4000.0f, 14800.0f);
						next = true;
					}
					else if (bodies < 16000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 13000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(1.5f*body_radius, 0.0f));
					}
					else if (!next && bodies == 16011) {
						delay = 0.01f;
						nb = 20;
						zoom = 0.1f;
						offset = up::Vec2(4000.0f, 12000.0f);
						next = true;
					}
					else if (bodies < 32000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 12000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(1.5f*body_radius, 0.0f));
					}
					else if (!next && bodies == 32011) {
						delay = 0.00f;
						nb = 20;
						zoom = 0.07f;
						offset = up::Vec2(6000.0f, 10000.0f);
						next = true;
					}
					else if (bodies < 50000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 10000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(1.75f*body_radius, 0.0f));
					}
					else if (!next && bodies == 50011) {
						delay = 0.00f;
						nb = 40;
						zoom = 0.055f;
						offset = up::Vec2(6000.0f, 8000.0f);
						next = true;
					}
					else if (bodies < 100000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 5000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(2.5f*body_radius, 0.0f));
					}
					else if (!next && bodies == 100019) {
						delay = 0.00f;
						nb = 40;
						zoom = 0.055f;
						offset = up::Vec2(8000.0f, 8000.0f);
						next = true;
						double_flow = true;
					}
					else if (bodies < 130000) {
						next = false;
						up::BodyPtr b = solver.addBody(up::Vec2(2.0f*body_radius, 5000 - 2 * i*body_radius));
						b->setVelocity(up::Vec2(2.5f*body_radius, 0.0f));

						up::BodyPtr b1 = solver.addBody(up::Vec2(16000 - 2.0f*body_radius, 5000 - 2 * i*body_radius));
						b1->setVelocity(up::Vec2(-2.5f*body_radius, 0.0f));
					}
					else if (!next && bodies == 130011) {
						delay = 0.00f;
						nb = 40;
						offset = up::Vec2(8000.0f, 8000.0f);
						next = true;
						double_flow = false;
					}
					else if (bodies < 200000) {
						next = false;
						up::BodyPtr b1 = solver.addBody(up::Vec2(16000 - 2.0f*body_radius, 5000 - 2 * i*body_radius));
						b1->setVelocity(up::Vec2(-2.5f*body_radius, 0.0f));
					}
					else {
						std::cout << bodies << std::endl;
						bodies = 20000000;
					}
				}

				bodies += nb;
				if (double_flow) {
					bodies += nb;
				}
			}
		}
		else if (displayManager.emit) {
			bodies = 180000;
		}

		displayManager.processEvents();

		if (displayManager.clic) {
			displayManager.clic = false;
			//addSolidSegment(solver, world_coord.x, world_coord.y, world_coord.x, world_coord.y + 100.0f);
			//addBox(solver, world_coord.x, world_coord.y, 50.0f + rand()%200, 50.0f + rand() % 200);
			//addBox(solver, world_coord.x, world_coord.y, 100.0f, 100.0f);
		}

		if (1 || displayManager.emit) {
			solver.update(0.016f);
		}

		render_tex.clear(sf::Color::White);
		window.clear(sf::Color::White);

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