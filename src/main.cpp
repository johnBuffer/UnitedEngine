#include <SFML/Graphics.hpp>

#include "united_solver.hpp"
#include "display_manager.hpp"
#include <sstream>
#include <stdlib.h>
#include "utils.hpp"
#include "segment.hpp"
#include <dynamic_blur.hpp>
#include <iostream>


int main()
{
	const uint32_t win_width = 1920;
	const uint32_t win_height = 1080;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Fullscreen, settings);
	//window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	const float body_radius(16.0f);
	up::Vec2 world_dimension(12860.0f, 16000.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 980.0f });

	sf::RenderTexture render_tex;
	render_tex.create(win_width, win_height);

	DisplayManager displayManager(render_tex, window, solver);

	sf::Font font;
	font.loadFromFile("res/font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(32);
	text.setFillColor(sf::Color(250, 250, 250));

	sf::Clock clock, spawn_timer;
	Blur blur(win_width, win_height, 0.5f);

	bool double_flow(false);
	uint32_t nb(1);
	float delay(0.5f);
	bool next(false);
	uint32_t bodies(0);
	float acc_time = 0.0f;
	while (window.isOpen())
	{
		const sf::Vector2i mouse_pos(sf::Mouse::getPosition(window));
		const up::Vec2 world_coord(displayManager.displayCoordToWorldCoord(up::Vec2(mouse_pos.x, mouse_pos.y)));

		clock.restart();

		uint32_t n(400);

		if (displayManager.emit && bodies < 200000) {
			if (acc_time > 70.0f) {
				const float spawn_width = n * 2.0f * body_radius;
				for (int i(0); i < n; ++i) {
					solver.addBody(up::Vec2(world_dimension.x * 0.5f - spawn_width *0.5f + i * 2.0f *body_radius + rand() % 2, 10.0f));
				}
				acc_time = 0.0f;
				bodies += n;
			}
			if (displayManager.update) {
				acc_time += 16.0f;
			}
		}

		displayManager.processEvents(window);

		if (displayManager.explosion)
		{
			solver.addExplosion(world_coord);
			displayManager.explosion = false;
		}

		if (displayManager.clic) {
			up::Vec2 clic_pos_display_coord = displayManager.getClicPosition();
			up::Vec2 clic_pos_world_coord = displayManager.displayCoordToWorldCoord(up::Vec2(mouse_pos.x, mouse_pos.y));
		}

		if (displayManager.clic) {
			displayManager.clic = false;
		}

		if (displayManager.update) {
			solver.update(0.016f);
		}

		render_tex.clear(sf::Color(80, 80, 80));

		displayManager.draw(false);

		render_tex.display();
		window.draw(sf::Sprite(render_tex.getTexture()));
		blur.setRegion(10, 10, 450, 170);
		window.draw(blur.apply(render_tex.getTexture(), 3));
		sf::RectangleShape rec(sf::Vector2f(450, 170));
		rec.setPosition(10, 10);
		rec.setFillColor(sf::Color(50, 50, 50, 100));
		window.draw(rec);

		text.setString("Objects: " + to_string(bodies));
		text.setCharacterSize(32);
		text.setPosition(25, 15);
		window.draw(text);

		text.setCharacterSize(24);
		text.setString("Physics time: " + round(solver.getCollisionTime() + solver.getGridTime(), 2) + " ms");
		text.setPosition(20, 70);
		window.draw(text);

		text.setString("Render time : " + round(displayManager.render_time, 2) + " ms");
		text.setPosition(20, 102);
		window.draw(text);

		text.setString("Frame time  : " + round(clock.getElapsedTime().asMilliseconds(), 2) + " ms");
		text.setPosition(20, 134);
		window.draw(text);

		window.display();
	}

	return 0;
}