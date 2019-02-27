#include "DisplayManager.h"
#include <iostream>

DisplayManager::DisplayManager(sf::RenderWindow *window, up::UnitedSolver* collisionManager)
{
    m_window = window;
    m_collisionManager = collisionManager;

    m_zoom = 1;
    m_offsetX = 0;
    m_offsetY = 0;

    m_windowOffsetX = m_window->getSize().x/2;
    m_windowOffsetY = m_window->getSize().y/2;

    m_bodyTexture.loadFromFile("circle.png");
}

up::Vec2 DisplayManager::worldCoordToDisplayCoord(const up::Vec2& worldCoord)
{
    double worldX = worldCoord.x;
    double worldY = worldCoord.y;

    double viewCoordX = (worldX+m_offsetX)*m_zoom+m_windowOffsetX;
    double viewCoordY = (worldY+m_offsetY)*m_zoom+m_windowOffsetY;

    return up::Vec2(viewCoordX, viewCoordY);
}

up::Vec2 DisplayManager::displayCoordToWorldCoord(const up::Vec2& viewCoord)
{
    double viewCoordX = viewCoord.x;
    double viewCoordY = viewCoord.y;

    double worldCoordX = (viewCoordX-m_windowOffsetX)/m_zoom-m_offsetX;
    double worldCoordY = (viewCoordY-m_windowOffsetY)/m_zoom-m_offsetY;

    return up::Vec2(worldCoordX, worldCoordY);
}

void DisplayManager::draw(bool showInner)
{
    // draw the world's ground as a big black square
    sf::RectangleShape ground(sf::Vector2f(m_collisionManager->dimension().x*m_zoom, m_collisionManager->dimension().y*m_zoom));
    ground.setFillColor(sf::Color::Black);
    ground.setPosition(m_offsetX*m_zoom+m_windowOffsetX, m_offsetY*m_zoom+m_windowOffsetY);
    m_window->draw(ground);

    // draw the guys
	const fva::SwapArray<up::Body>& bodies_data = m_collisionManager->bodies();
    int bodyCount = bodies_data.size();
    sf::VertexArray bodies(sf::Quads, 4*bodyCount);
	uint32_t i(0);
    for (const up::Body& body : bodies_data)
    {
        double radius = body.radius()*m_zoom;

        up::Vec2 viewCoord = worldCoordToDisplayCoord(body.position());

        bodies[4*i  ].position = sf::Vector2f(viewCoord.x-radius, viewCoord.y-radius);
        bodies[4*i+1].position = sf::Vector2f(viewCoord.x+radius, viewCoord.y-radius);
        bodies[4*i+2].position = sf::Vector2f(viewCoord.x+radius, viewCoord.y+radius);
        bodies[4*i+3].position = sf::Vector2f(viewCoord.x-radius, viewCoord.y+radius);

        bodies[4*i  ].texCoords = sf::Vector2f(0, 0);
        bodies[4*i+1].texCoords = sf::Vector2f(512, 0);
        bodies[4*i+2].texCoords = sf::Vector2f(512, 512);
        bodies[4*i+3].texCoords = sf::Vector2f(0, 512);

		/*const float pi = 3.1415926f;

		float t = i / 1000.0f;
		float r = sin(t)*sin(t);
		float g = sin(t + 0.5f*pi)*sin(t + 0.5f*pi);
		float b = sin(0.1*t)*sin(0.1*t);
		sf::Color color(255*r, 255*g, 255*b);
		bodies[4 * i].color = color;
		bodies[4 * i + 1].color = color;
		bodies[4 * i + 2].color = color;
		bodies[4 * i + 3].color = color;*/

		/*float pressure = std::min(body.mass(), 255.0f);
		sf::Color color(255, 255 - pressure, 255 - pressure);
		bodies[4 * i].color = color;
		bodies[4 * i + 1].color = color;
		bodies[4 * i + 2].color = color;
		bodies[4 * i + 3].color = color;*/

		++i;
    }
    m_window->draw(bodies, &m_bodyTexture);
}

void DisplayManager::processEvents()
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(*m_window);

	sf::Event event;
	while (m_window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape) m_window->close();
			else if ((event.key.code == sf::Keyboard::Subtract)) zoom(0.8);
			else if ((event.key.code == sf::Keyboard::Add)) zoom(1.2);
			//else if ((event.key.code == sf::Keyboard::Space)) emit = !emit;
			break;
		case sf::Event::MouseWheelMoved:
			// this is an amazing zoom
			zoom(1 + event.mouseWheel.delta / 5.0);
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				_mouse_button_pressed = true;
				_drag_clic_position = mousePosition;
				_clic_position = mousePosition;
			}

			break;
		case sf::Event::MouseButtonReleased:
			_mouse_button_pressed = false;
			if (_clic_position == mousePosition)
			{
				m_collisionManager->applyExplosion(displayCoordToWorldCoord(up::Vec2(_clic_position.x, _clic_position.y)), 10000000000.0f);
			}

			break;
		case sf::Event::MouseMoved:
			if (_mouse_button_pressed) // in this case we are dragging
			{
				// updating displayManager offset
				addOffset(mousePosition.x - _drag_clic_position.x, mousePosition.y - _drag_clic_position.y);
				_drag_clic_position = mousePosition;
			}
			break;
		default:
			break;
		}
	}
}

up::Body* DisplayManager::getBodyAt(double x, double y)
{
	return nullptr;// m_collisionManager->getBodyAt(displayCoordToWorldCoord(up::Vec2(x, y)));
}
