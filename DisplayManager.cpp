#include "DisplayManager.h"
#include <iostream>
#include "PSprite.h"

DisplayManager::DisplayManager(sf::RenderWindow *window, U_2DCollisionManager *collisionManager)
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

U_2DCoord DisplayManager::worldCoordToDisplayCoord(const U_2DCoord& worldCoord)
{
    double worldX = worldCoord.x;
    double worldY = worldCoord.y;

    double viewCoordX = (worldX+m_offsetX)*m_zoom+m_windowOffsetX;
    double viewCoordY = (worldY+m_offsetY)*m_zoom+m_windowOffsetY;

    return U_2DCoord(viewCoordX, viewCoordY);
}

U_2DCoord DisplayManager::displayCoordToWorldCoord(const U_2DCoord& viewCoord)
{
    double viewCoordX = viewCoord.x;
    double viewCoordY = viewCoord.y;

    double worldCoordX = (viewCoordX-m_windowOffsetX)/m_zoom-m_offsetX;
    double worldCoordY = (viewCoordY-m_windowOffsetY)/m_zoom-m_offsetY;

    return U_2DCoord(worldCoordX, worldCoordY);
}

void DisplayManager::draw(bool showInner)
{
    // draw the world's ground as a big black square
    sf::RectangleShape ground(sf::Vector2f(m_collisionManager->getMapSize().x*m_zoom, m_collisionManager->getMapSize().y*m_zoom));
    ground.setFillColor(sf::Color::Black);
    ground.setPosition(m_offsetX*m_zoom+m_windowOffsetX, m_offsetY*m_zoom+m_windowOffsetY);
    m_window->draw(ground);

    // draw the guys
    int bodyCount = m_collisionManager->getBodiesCount();
    sf::VertexArray bodies(sf::Quads, 4*bodyCount);
    for (int i(0); i<bodyCount; ++i)
    {
        U_2DBody* currentBody = m_collisionManager->getBodyAt(i);

        double radius = currentBody->getRadius()*m_zoom;

        U_2DCoord viewCoord = worldCoordToDisplayCoord(currentBody->getPosition());

        bodies[4*i  ].position = sf::Vector2f(viewCoord.x-radius, viewCoord.y-radius);
        bodies[4*i+1].position = sf::Vector2f(viewCoord.x+radius, viewCoord.y-radius);
        bodies[4*i+2].position = sf::Vector2f(viewCoord.x+radius, viewCoord.y+radius);
        bodies[4*i+3].position = sf::Vector2f(viewCoord.x-radius, viewCoord.y+radius);

        bodies[4*i  ].texCoords = sf::Vector2f(0, 0);
        bodies[4*i+1].texCoords = sf::Vector2f(512, 0);
        bodies[4*i+2].texCoords = sf::Vector2f(512, 512);
        bodies[4*i+3].texCoords = sf::Vector2f(0, 512);
    }
    m_window->draw(bodies, &m_bodyTexture);

    int n_constr = m_collisionManager->getConstraintsCount();
    sf::VertexArray constrs(sf::Lines, 2*n_constr);
    int i(0);
    for (auto& ctr : m_collisionManager->getConstraints())
    {
        U_2DCoord viewCoord1 = worldCoordToDisplayCoord(ctr->getBody1Pos());
        U_2DCoord viewCoord2 = worldCoordToDisplayCoord(ctr->getBody2Pos());

        constrs[2*i].position = sf::Vector2f(viewCoord1.x, viewCoord1.y);
        constrs[2*i+1].position = sf::Vector2f(viewCoord2.x, viewCoord2.y);
        ++i;
    }
    m_window->draw(constrs);

    int n_springs = m_collisionManager->getSpringsCount();
    sf::VertexArray springs(sf::Lines, 2*n_springs);
    for (int i(0); i<n_springs; ++i)
    {
        U_2DSpring* sprng = m_collisionManager->getSpringAt(i);

        U_2DCoord viewCoord1 = worldCoordToDisplayCoord(sprng->getBody1Pos());
        U_2DCoord viewCoord2 = worldCoordToDisplayCoord(sprng->getBody2Pos());

        springs[2*i].position = sf::Vector2f(viewCoord1.x, viewCoord1.y);
        springs[2*i].color = sf::Color::Green;
        springs[2*i+1].position = sf::Vector2f(viewCoord2.x, viewCoord2.y);
        springs[2*i+1].color = sf::Color::Green;
    }
    //m_window->draw(springs);

    for (auto &ps : _psprites)
    {
        ps.update();
        sf::Sprite& sp = ps.getSprite();
        sf::Vector2f pos = sp.getPosition();
        U_2DCoord viewPos = worldCoordToDisplayCoord(U_2DCoord(pos.x, pos.y));
        sp.scale(1.1*m_zoom, 1.1*m_zoom);
        sp.setPosition(viewPos.x, viewPos.y);
        m_window->draw(sp);
    }
}

U_2DBody* DisplayManager::getBodyAt(double x, double y)
{
    return m_collisionManager->getBodyAt(displayCoordToWorldCoord(U_2DCoord(x, y)));
}
