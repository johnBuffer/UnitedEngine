#include "display_manager.hpp"
#include <iostream>

DisplayManager::DisplayManager(sf::RenderWindow& window, up::UnitedSolver& solver) :
	m_window(window),
	m_solver(solver),
	m_event_manager(window),
	m_zoom(1.0f),
	m_offsetX(0.0f),
	m_offsetY(0.0f)
{
	m_windowOffsetX = m_window.getSize().x * 0.5f;
    m_windowOffsetY = m_window.getSize().y * 0.5f;

    m_bodyTexture.loadFromFile("circle.png");

	m_show_pressure = false;
}

up::Vec2 DisplayManager::worldCoordToDisplayCoord(const up::Vec2& worldCoord)
{
    float worldX = worldCoord.x;
    float worldY = worldCoord.y;

    float viewCoordX = (worldX-m_offsetX)*m_zoom+m_windowOffsetX;
    float viewCoordY = (worldY-m_offsetY)*m_zoom+m_windowOffsetY;

    return up::Vec2(viewCoordX, viewCoordY);
}

up::Vec2 DisplayManager::displayCoordToWorldCoord(const up::Vec2& viewCoord)
{
    float viewCoordX = viewCoord.x;
    float viewCoordY = viewCoord.y;

    float worldCoordX = (viewCoordX-m_windowOffsetX)/m_zoom+m_offsetX;
    float worldCoordY = (viewCoordY-m_windowOffsetY)/m_zoom+m_offsetY;

    return up::Vec2(worldCoordX, worldCoordY);
}

void DisplayManager::draw(bool showInner)
{
	sf::Clock clock;
    // draw the world's ground as a big black square
    sf::RectangleShape ground(sf::Vector2f(m_solver.getDimension().x, m_solver.getDimension().y));
    ground.setFillColor(sf::Color::Black);

	sf::RenderStates rs_ground;
	rs_ground.transform.translate(m_windowOffsetX, m_windowOffsetY);
	rs_ground.transform.scale(m_zoom, m_zoom);
	rs_ground.transform.translate(-m_offsetX, -m_offsetY);
    m_window.draw(ground, rs_ground);

    // draw the guys
	const fva::SwapArray<up::Body>& bodies_data = m_solver.getBodies();
    int bodyCount = bodies_data.size();
    sf::VertexArray bodies(sf::Quads, 4*bodyCount);
	uint32_t i(0);
    for (const up::Body& body : bodies_data)
    {
        float radius = body.radius()*1.0f;

		const up::Vec2& position = body.position();

		bodies[4 * i].position = sf::Vector2f(position.x - radius, position.y - radius);
		bodies[4 * i + 1].position = sf::Vector2f(position.x + radius, position.y - radius);
		bodies[4 * i + 2].position = sf::Vector2f(position.x + radius, position.y + radius);
		bodies[4 * i + 3].position = sf::Vector2f(position.x - radius, position.y + radius);

        bodies[4*i  ].texCoords = sf::Vector2f(0, 0);
        bodies[4*i+1].texCoords = sf::Vector2f(512, 0);
        bodies[4*i+2].texCoords = sf::Vector2f(512, 512);
        bodies[4*i+3].texCoords = sf::Vector2f(0, 512);


		if (!m_show_pressure)
		{
			const float pi = 3.1415926f;
			float t = i / 1000.0f;
			float r = sin(t);
			float g = sin(t + 0.33f * 2 * pi);
			float b = sin(t + 0.66f * 2 * pi);
			sf::Color color(uint8_t(255*r*r), uint8_t(255*g*g), uint8_t(255*b*b));
			bodies[4 * i].color = color;
			bodies[4 * i + 1].color = color;
			bodies[4 * i + 2].color = color;
			bodies[4 * i + 3].color = color;
		}
		else
		{
			float pressure = std::min(body.mass()*5.0F, 255.0f);
			sf::Color color(255, uint8_t(255 - pressure), uint8_t(255 - pressure));
			bodies[4 * i].color = color;
			bodies[4 * i + 1].color = color;
			bodies[4 * i + 2].color = color;
			bodies[4 * i + 3].color = color;
		}

		++i;
	}

	sf::RenderStates rs;
	rs.texture = &m_bodyTexture;
	rs.transform.translate(m_windowOffsetX, m_windowOffsetY);
	rs.transform.scale(m_zoom, m_zoom);
	rs.transform.translate(-m_offsetX, -m_offsetY);

    m_window.draw(bodies, rs);

	drawConstraints(m_solver.getConstraints());

	const auto& segments(m_solver.getSegments());
	for (const up::SolidSegment& s : segments) {
		drawSegment(s);
	}

	render_time = clock.getElapsedTime().asMicroseconds() * 0.001f;
}

void DisplayManager::processEvents()
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);

	//m_event_manager.processEvents();

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape) m_window.close();
			else if ((event.key.code == sf::Keyboard::Subtract)) zoom(0.8f);
			else if ((event.key.code == sf::Keyboard::Add)) zoom(1.2f);
			else if ((event.key.code == sf::Keyboard::Space)) emit = !emit;
			else if ((event.key.code == sf::Keyboard::A)) m_show_pressure = !m_show_pressure;
			else if ((event.key.code == sf::Keyboard::R))
			{
				m_offsetX = 0.0f;
				m_offsetY = 0.0f;
				m_zoom = 1.0f;
			}
			break;
		case sf::Event::MouseWheelMoved:
			// this is an amazing zoom
			zoom(1 + event.mouseWheel.delta * 0.2f);
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_mouse_button_pressed = true;
				m_drag_clic_position = mousePosition;
				m_clic_position = mousePosition;
			}

			break;
		case sf::Event::MouseButtonReleased:
			m_mouse_button_pressed = false;
			if (m_clic_position == mousePosition)
			{
				clic = true;
			}

			break;
		case sf::Event::MouseMoved:
			if (m_mouse_button_pressed) // in this case we are dragging
			{
				// updating displayManager offset
				const float vx = float(mousePosition.x - m_drag_clic_position.x);
				const float vy = float(mousePosition.y - m_drag_clic_position.y);
				addOffset(vx, vy);
				m_drag_clic_position = mousePosition;
			}
			break;
		default:
			break;
		}
	}
}

up::Body* DisplayManager::getBodyAt(float x, float y)
{
	return nullptr;// m_collisionManager->getBodyAt(displayCoordToWorldCoord(up::Vec2(x, y)));
}

void DisplayManager::drawConstraints(const fva::SwapArray<up::Constraint>& constraints)
{
	sf::VertexArray cva(sf::Lines, constraints.size() * 2);
	uint32_t i(0);
	for (const up::Constraint& c : constraints)
	{
		const up::Vec2& p1 = worldCoordToDisplayCoord(c.position1());
		const up::Vec2& p2 = worldCoordToDisplayCoord(c.position2());

		cva[2 * i + 0].position = sf::Vector2f(p1.x, p1.y);
		cva[2 * i + 1].position = sf::Vector2f(p2.x, p2.y);

		++i;
	}

	m_window.draw(cva);
}

void DisplayManager::drawPoint(const up::Vec2& point)
{
	const up::Vec2 p(worldCoordToDisplayCoord(point));

	const float radius(4.0f);
	sf::CircleShape c(radius);
	c.setOrigin(radius, radius);
	c.setPosition(p.x, p.y);
	c.setFillColor(sf::Color::Red);

	m_window.draw(c);
}

void DisplayManager::drawSegment(const up::SolidSegment& segment)
{
	const up::Vec2& b1p(segment.getBody1Position());
	const up::Vec2& b2p(segment.getBody2Position());
	
	const up::Vec2 wb1p(worldCoordToDisplayCoord(b1p));
	const up::Vec2 wb2p(worldCoordToDisplayCoord(b2p));

	sf::VertexArray va(sf::Lines, 2);
	va[0].position = sf::Vector2f(wb1p.x, wb1p.y);
	va[1].position = sf::Vector2f(wb2p.x, wb2p.y);
	
	m_window.draw(va);
}

