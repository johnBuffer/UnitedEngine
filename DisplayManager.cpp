#include "DisplayManager.h"
#include <iostream>

DisplayManager::DisplayManager(sf::RenderWindow *window, UnitedSolver* collisionManager)
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

Vec2 DisplayManager::worldCoordToDisplayCoord(const Vec2& worldCoord)
{
    double worldX = worldCoord.x;
    double worldY = worldCoord.y;

    double viewCoordX = (worldX+m_offsetX)*m_zoom+m_windowOffsetX;
    double viewCoordY = (worldY+m_offsetY)*m_zoom+m_windowOffsetY;

    return Vec2(viewCoordX, viewCoordY);
}

Vec2 DisplayManager::displayCoordToWorldCoord(const Vec2& viewCoord)
{
    double viewCoordX = viewCoord.x;
    double viewCoordY = viewCoord.y;

    double worldCoordX = (viewCoordX-m_windowOffsetX)/m_zoom-m_offsetX;
    double worldCoordY = (viewCoordY-m_windowOffsetY)/m_zoom-m_offsetY;

    return Vec2(worldCoordX, worldCoordY);
}

void DisplayManager::draw(bool showInner)
{
    // draw the world's ground as a big black square
    sf::RectangleShape ground(sf::Vector2f(m_collisionManager->dimension().x*m_zoom, m_collisionManager->dimension().y*m_zoom));
    ground.setFillColor(sf::Color::Black);
    ground.setPosition(m_offsetX*m_zoom+m_windowOffsetX, m_offsetY*m_zoom+m_windowOffsetY);
    m_window->draw(ground);

    // draw the guys
    int bodyCount = m_collisionManager->bodyCount();
    sf::VertexArray bodies(sf::Quads, 4*bodyCount);
	uint32_t i(0);
    for (const Body& body : m_collisionManager->bodies())
    {
        double radius = body.radius()*m_zoom;

        Vec2 viewCoord = worldCoordToDisplayCoord(body.position());

        bodies[4*i  ].position = sf::Vector2f(viewCoord.x-radius, viewCoord.y-radius);
        bodies[4*i+1].position = sf::Vector2f(viewCoord.x+radius, viewCoord.y-radius);
        bodies[4*i+2].position = sf::Vector2f(viewCoord.x+radius, viewCoord.y+radius);
        bodies[4*i+3].position = sf::Vector2f(viewCoord.x-radius, viewCoord.y+radius);

        bodies[4*i  ].texCoords = sf::Vector2f(0, 0);
        bodies[4*i+1].texCoords = sf::Vector2f(512, 0);
        bodies[4*i+2].texCoords = sf::Vector2f(512, 512);
        bodies[4*i+3].texCoords = sf::Vector2f(0, 512);

		const float pi = 3.1415926f;

		float t = body.id() / 1000.0f;
		float r = sin(t)*sin(t);
		float g = sin(t + 0.5f*pi)*sin(t + 0.5f*pi);
		float b = sin(0.1*t)*sin(0.1*t);
		sf::Color color(255*r, 255*g, 255*b);
		bodies[4 * i].color = color;
		bodies[4 * i + 1].color = color;
		bodies[4 * i + 2].color = color;
		bodies[4 * i + 3].color = color;

		++i;
    }
    m_window->draw(bodies, &m_bodyTexture);
}

Body* DisplayManager::getBodyAt(double x, double y)
{
	return nullptr;// m_collisionManager->getBodyAt(displayCoordToWorldCoord(Vec2(x, y)));
}
