#include "display_manager.hpp"
#include <iostream>

DisplayManager::DisplayManager(sf::RenderTarget& target, sf::RenderWindow& window, up::UnitedSolver& solver) :
	m_window(window),
	m_target(target),
	m_solver(solver),
	m_event_manager(window),
	m_zoom(1.0f),
	m_offsetX(0.0f),
	m_offsetY(0.0f),
	speed_mode(false)
	, m_swarm(16)
	, m_va(sf::Quads, 0)
	, update(true)
	, debug_mode(false)
{
	m_windowOffsetX = m_window.getSize().x * 0.5f;
    m_windowOffsetY = m_window.getSize().y * 0.5f;

    m_bodyTexture.loadFromFile("circle.png");

	m_show_pressure = false;

	//m_swarm.setJob([this](std::vector<up::Body>& data, uint32_t id, uint32_t step) {updateVertexArray(data, id, step); });
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
    m_target.draw(ground, rs_ground);

    // draw the guys
	const fva::SwapArray<up::Body>& bodies_data = m_solver.getBodies();
    int bodyCount = bodies_data.size();
	m_va.resize(4 * bodyCount);
	m_swarm.execute([&](uint32_t id, uint32_t worker_count) {updateVertexArray(bodies_data.getConstData(), id, worker_count); });
	m_swarm.waitExecutionDone();

	sf::RenderStates rs;
	rs.texture = &m_bodyTexture;
	rs.transform.translate(m_windowOffsetX, m_windowOffsetY);
	rs.transform.scale(m_zoom, m_zoom);
	rs.transform.translate(-m_offsetX, -m_offsetY);

    m_target.draw(m_va, rs);

	drawConstraints(m_solver.getConstraints());


	const auto& segments(m_solver.getSegments());
	for (const up::SolidSegment& s : segments) {
		drawSegment(s);
	}

	if (debug_mode)
		drawGrid(m_solver.getGrid(), rs_ground);

	render_time = clock.getElapsedTime().asMicroseconds() * 0.001f;
}

void DisplayManager::updateVertexArray(const std::vector<up::Body>& bodies, uint32_t id, uint32_t step)
{
	const uint32_t size(bodies.size());

	if (!size) {
		return;
	}

	uint32_t step_size(size / step);
	uint32_t start_index(id * step_size);
	uint32_t end_index(start_index + step_size);
	if (id == step-1) {
		end_index = size;
	}

	for (uint32_t i(start_index); i<end_index; ++i) {
		const up::Body& body(bodies[i]);
		float radius = body.radius;
		const up::Vec2& position = body.position();

		m_va[4 * i + 0].position = sf::Vector2f(position.x - radius, position.y - radius);
		m_va[4 * i + 1].position = sf::Vector2f(position.x + radius, position.y - radius);
		m_va[4 * i + 2].position = sf::Vector2f(position.x + radius, position.y + radius);
		m_va[4 * i + 3].position = sf::Vector2f(position.x - radius, position.y + radius);

		m_va[4 * i + 0].texCoords = sf::Vector2f(0, 0);
		m_va[4 * i + 1].texCoords = sf::Vector2f(512, 0);
		m_va[4 * i + 2].texCoords = sf::Vector2f(512, 512);
		m_va[4 * i + 3].texCoords = sf::Vector2f(0, 512);

		const float pi = 3.1415926f;
		float t = i / 1000.0f;
		float r = sin(t);
		float g = sin(t + 0.33f * 2 * pi);
		float b = sin(t + 0.66f * 2 * pi);

		sf::Color color = sf::Color(255*r*r, 255*g*g, 255*b*b);

		if (m_show_pressure) {
			float r = std::min(255.0f, body.move_acc*10.0f);
			color = sf::Color(r, 0, 0);
		}

		if (body.debug) {
			color = sf::Color::Red;
		} else if (body.debug_collision) {
			color = sf::Color::Green;
		}

		m_va[4 * i + 0].color = color;
		m_va[4 * i + 1].color = color;
		m_va[4 * i + 2].color = color;
		m_va[4 * i + 3].color = color;
		/*} else {
			float pressure = std::min(body.mass()*5.0F, 255.0f);
			sf::Color color(255, uint8_t(255 - pressure), uint8_t(255 - pressure));
			m_va[4 * i].color = color;
			m_va[4 * i + 1].color = color;
			m_va[4 * i + 2].color = color;
			m_va[4 * i + 3].color = color;
		}*/
	}
}

void DisplayManager::processEvents()
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);

	//m_event_manager.processEvents();

	clic = false;

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape) m_window.close();
			else if ((event.key.code == sf::Keyboard::Subtract)) zoom(0.8f);
			else if ((event.key.code == sf::Keyboard::Add)) zoom(1.2f);
			else if ((event.key.code == sf::Keyboard::Space)) update = !update;
			else if ((event.key.code == sf::Keyboard::A)) m_show_pressure = !m_show_pressure;
			else if ((event.key.code == sf::Keyboard::E)) emit = !emit;
			else if ((event.key.code == sf::Keyboard::D)) debug_mode = !debug_mode;
			else if ((event.key.code == sf::Keyboard::R))
			{
				m_offsetX = 0.0f;
				m_offsetY = 0.0f;
				m_zoom = 1.0f;
			}
			else if ((event.key.code == sf::Keyboard::E))
			{
				speed_mode = !speed_mode;
				m_window.setVerticalSyncEnabled(!speed_mode);
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

	m_target.draw(cva);
}

void DisplayManager::drawPoint(const up::Vec2& point)
{
	const up::Vec2 p(worldCoordToDisplayCoord(point));

	const float radius(4.0f);
	sf::CircleShape c(radius);
	c.setOrigin(radius, radius);
	c.setPosition(p.x, p.y);
	c.setFillColor(sf::Color::Red);

	m_target.draw(c);
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
	
	m_target.draw(va);
}

void DisplayManager::drawGrid(const up::Grid<GRID_CELL_SIZE>& grid, const sf::RenderStates& state)
{
	const up::Vec2 grid_size(grid.getSize());
	const float cell_size(grid.getCellSize());

	sf::VertexArray va(sf::Quads, 4 * grid_size.x * grid_size.y);
	for (int32_t x(0); x < grid_size.x; ++x) {
		for (int32_t y(0); y < grid_size.y; ++y) {
			va[4 * grid_size.y * x + 4 * y + 0].position = sf::Vector2f((x - 5) * cell_size, (y - 5) * cell_size);
			va[4 * grid_size.y * x + 4 * y + 1].position = sf::Vector2f((x - 4) * cell_size, (y - 5) * cell_size);
			va[4 * grid_size.y * x + 4 * y + 2].position = sf::Vector2f((x - 4) * cell_size, (y - 4) * cell_size);
			va[4 * grid_size.y * x + 4 * y + 3].position = sf::Vector2f((x - 5) * cell_size, (y - 4) * cell_size);

			sf::Color color(0, 0, 0, 0);
			if (grid.getCellAt(x, y).segment_count) {
				color.a = 128;
				color.r = 255;
			}

			if (grid.getCellAt(x, y).item_count) {
				color.a = 128;
				color.g = 255;
			}

			va[4 * grid_size.y * x + 4 * y + 0].color = color;
			va[4 * grid_size.y * x + 4 * y + 1].color = color;
			va[4 * grid_size.y * x + 4 * y + 2].color = color;
			va[4 * grid_size.y * x + 4 * y + 3].color = color;
		}
	}

	m_target.draw(va, state);

	const uint32_t h_count = grid_size.y + 1;
	const uint32_t v_count = grid_size.x + 1;
	const uint32_t line_count = h_count + v_count;
	sf::VertexArray va_lines(sf::Lines, 2 * line_count);

	sf::Color color = sf::Color(100, 100, 100);

	for (int32_t x(0); x < v_count + 1; ++x) {
		va_lines[2 * x + 0].position = sf::Vector2f((x-5)*cell_size, -5*cell_size);
		va_lines[2 * x + 1].position = sf::Vector2f((x-5)*cell_size, (grid_size.y-5)*cell_size);

		va_lines[2 * x + 0].color = color;
		va_lines[2 * x + 1].color = color;
	}

	for (int32_t y(0); y < h_count; ++y) {
		va_lines[2 * v_count + 2 * y + 0].position = sf::Vector2f(-5 * cell_size, (y-5)*cell_size);
		va_lines[2 * v_count + 2 * y + 1].position = sf::Vector2f((grid_size.x-5)*cell_size, (y-5)*cell_size);

		va_lines[2 * v_count + 2 * y + 0].color = color;
		va_lines[2 * v_count + 2 * y + 1].color = color;
	}

	m_target.draw(va_lines, state);

}
