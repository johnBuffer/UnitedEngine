#include "U_2DBody.h"
#include <iostream>

U_2DBody::U_2DBody()
{
    init(U_2DCoord(), 0, 0, false);
}

U_2DBody::U_2DBody(U_2DCoord position, double mass, bool included)
{
    init(position, 0, mass, included);
}

U_2DBody::U_2DBody(U_2DCoord position, double radius, double mass, bool included)
{
    init(position, radius, mass, included);
}

void U_2DBody::init(U_2DCoord position, double radius, double mass, bool included)
{
    m_position = position;
    m_lastPosition = position;
    m_mass = mass;
    m_acceleration = U_2DCoord();
    m_static = false;
    m_included = false;
    m_pressure = 0;
    m_penetrationVelocity = 10000000000;
    m_gridChange = true;
    m_radius = radius;
}

void U_2DBody::move2D(U_2DCoord vec)
{
    if (!m_static)
        m_position.move2D(vec);
}

void U_2DBody::moveLastPos2D(U_2DCoord vec)
{
    m_lastPosition.move2D(vec);
}

void U_2DBody::accelerate2D(U_2DCoord vec)
{
    m_acceleration.move2D(vec);
}

void U_2DBody::updatePosition(double timeStep)
{
    U_2DCoord newPosition;
    newPosition.x = m_position.x+(m_position.x-m_lastPosition.x)+timeStep*m_acceleration.x;
    newPosition.y = m_position.y+(m_position.y-m_lastPosition.y)+timeStep*m_acceleration.y;

    if (!m_static)
    {
        m_lastPosition = m_position;
        m_position = newPosition;
    }

    m_acceleration = {};
}

U_2DCoord U_2DBody::getVelocity() const
{
    return U_2DCoord(m_position.x - m_lastPosition.x, m_position.y - m_lastPosition.y);
}




