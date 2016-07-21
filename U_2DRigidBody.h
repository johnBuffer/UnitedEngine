#ifndef U_2DRIGIDBODY_H_INCLUDED
#define U_2DRIGIDBODY_H_INCLUDED

#include "U_2DBody.h"

class U_2DRigidBody
{
public:
    U_2DRigidBody(int width, int height);

    void setBoundsBodies(U_2DBody *body1, U_2DBody *body2, U_2DBody *body3, U_2DBody *body4);

    U_2DCoord getBody1Pos() const {return m_body1->getPosition();};
    U_2DCoord getBody2Pos() const {return m_body2->getPosition();};
    U_2DCoord getBody3Pos() const {return m_body3->getPosition();};
    U_2DCoord getBody4Pos() const {return m_body4->getPosition();};

    int getWidth() const {return m_width;};
    int getHeight() const {return m_height;};

private:
    U_2DBody *m_body1, *m_body2, *m_body3, *m_body4;
    int m_width, m_height;
};

#endif // U_2DRIGIDBODY_H_INCLUDED
