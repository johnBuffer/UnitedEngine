#include <SFML/Graphics.hpp>

#include "united_solver.h"
#include "DisplayManager.h"
#include <sstream>
#include "props.h"
#include <stdlib.h>
#include "utils.hpp"

#include <iostream>

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
	//window.setFramerateLimit(60);

	float body_radius = 2;
	up::Vec2 world_dimension(2000.0f, 500.0f);
	up::UnitedSolver solver(world_dimension, body_radius, { 0.0f, 900.0f });

	DisplayManager displayManager(&window, &solver);
	displayManager.setZoom(5);

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::White);

	displayManager.setOffset(50, 400);

	// Foot
	up::BodyPtr a = solver.addBody(50, 480);
	up::BodyPtr b = solver.addBody(80, 480);
	up::BodyPtr c = solver.addBody(60, 472);
	up::BodyPtr d = solver.addBody(55, 470);

	// Mollet
	up::BodyPtr e = solver.addBody(45, 430);
	up::BodyPtr f = solver.addBody(60, 430);
	up::BodyPtr g = solver.addBody(55, 415);
	up::BodyPtr g2 = solver.addBody(67, 410);

	// Cuisse
	up::BodyPtr h = solver.addBody(40, 355);
	up::BodyPtr i = solver.addBody(65, 355);
	//up::BodyPtr j = solver.addBody(55, 435);

	solver.addAnchor(h, 20);

	solver.addConstraint(a, b);
	solver.addConstraint(a, c);
	solver.addConstraint(a, d);
	solver.addConstraint(d, c);
	solver.addConstraint(c, b);
	solver.addConstraint(d, b);

	solver.addConstraint(e, f);
	solver.addConstraint(g, d);
	solver.addConstraint(g, e);
	solver.addConstraint(g, f);
	solver.addConstraint(g, g2);
	solver.addConstraint(g2, f);
	solver.addConstraint(g2, e);

	solver.addConstraint(e, d);
	solver.addConstraint(d, f);

	solver.addConstraint(h, g);
	solver.addConstraint(i, g);
	solver.addConstraint(h, i);

	auto muscle_ae = solver.addMuscle(e, a);
	solver.addMuscle(f, c);
	auto muscle_he = solver.addMuscle(h, e);
	solver.addMuscle(i, g2);

	displayManager.eventManager().addKeyPressedCallback(sf::Keyboard::A, [&](const sf::Event&) {muscle_ae->contract(0.5f); });
	displayManager.eventManager().addKeyReleasedCallback(sf::Keyboard::A, [&](const sf::Event&) {muscle_ae->contract(0.0f); });

	displayManager.eventManager().addKeyPressedCallback(sf::Keyboard::E, [&](const sf::Event&) {muscle_he->contract(0.8f); });
	displayManager.eventManager().addKeyReleasedCallback(sf::Keyboard::E, [&](const sf::Event&) {muscle_he->contract(0.0f); });

	while (window.isOpen())
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		displayManager.processEvents();

		solver.update(0.016f);

		window.clear(sf::Color::White);

		displayManager.draw(false);


		window.display();
	}

	return 0;
}