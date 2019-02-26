#include <SFML/Graphics.hpp>

#include "united_solver.h"
#include "DisplayManager.h"

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "UE2", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(false);

	Vec2 world_dimension(5000.0f, 10000.0f);
	UnitedSolver solver(world_dimension, 15, { 0.0f, 200.0f });

	DisplayManager displayManager(&window, &solver);
	displayManager.setOffset(-world_dimension.x / 2, -0.75*world_dimension.y);
	displayManager.setZoom(0.5);

	uint32_t n = 32000;
	uint32_t i = 0;

	bool emit = false;

	bool mouseButtonPressed = false;
	sf::Vector2i dragClicPosition, clicPosition;
	
	while (window.isOpen())
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape) window.close();
				else if ((event.key.code == sf::Keyboard::Subtract)) displayManager.zoom(0.8);
				else if ((event.key.code == sf::Keyboard::Add)) displayManager.zoom(1.2);
				else if ((event.key.code == sf::Keyboard::A)) solver.test_pressure = !solver.test_pressure;
				else if ((event.key.code == sf::Keyboard::Space)) emit = !emit;
				break;
			case sf::Event::MouseWheelMoved:
				// this is an amazing zoom
				displayManager.zoom(1 + event.mouseWheel.delta / 5.0);
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					mouseButtonPressed = true;
					dragClicPosition = mousePosition;
					clicPosition = mousePosition;
				}

				break;
			case sf::Event::MouseButtonReleased:
				mouseButtonPressed = false;
				if (clicPosition == mousePosition)
				{
					for (int i(500); --i;)
					{
						Vec2 pos(mousePosition.x + rand() % 50, mousePosition.y + rand() % 50);
						Vec2 worldPos = displayManager.displayCoordToWorldCoord(pos);
						solver.addBody(worldPos);
					}
				}

				break;
			case sf::Event::MouseMoved:
				if (mouseButtonPressed) // in this case we are dragging
				{
					// updating displayManager offset
					displayManager.addOffset(mousePosition.x - dragClicPosition.x, mousePosition.y - dragClicPosition.y);
					dragClicPosition = mousePosition;
				}
				break;
			default:
				break;
			}
		}

		if (i < n && emit)
		{
			i += 10;
			for (int ii(10); ii--;)
				solver.addBody({ float(rand() % 1000 + 1500), float(rand() % 100 + 9900) });
		}

		solver.update(0.016f);

		window.clear(sf::Color::White);

		displayManager.draw(false);

		window.display();
	}

	return 0;
}