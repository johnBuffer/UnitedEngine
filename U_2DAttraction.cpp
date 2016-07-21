#include "U_2DAttraction.h"

#include <stdlib.h>

U_2DAttraction::U_2DAttraction()
{
    m_body1 = NULL;
    m_body2 = NULL;
    m_strength = 0;
}

U_2DAttraction::U_2DAttraction(U_2DBody* b1, U_2DBody* b2, double strength)
{
    m_body1 = b1;
    m_body2 = b2;
    m_strength = strength;
}

void U_2DAttraction::applyAttraction()
{
    U_2DCoord v(m_body1->getPosition(), m_body2->getPosition());
    double dist = v.getNorm();
    double force = m_strength/(dist*dist+dist);
    m_body1->accelerate2D(U_2DCoord(v.x*force, v.y*force));
}
