#include "U_2DRigidBody.h"

U_2DRigidBody::U_2DRigidBody(int width, int height)
{
    m_width = width;
    m_height = height;
}

void U_2DRigidBody::setBoundsBodies(U_2DBody *body1, U_2DBody *body2, U_2DBody *body3, U_2DBody *body4)
{
    m_body1 = body1;
    m_body2 = body2;
    m_body3 = body3;
    m_body4 = body4;
}
