#include "U_2DSpring.h"


U_2DSpring::U_2DSpring(U_2DBody* body1, U_2DBody* body2, double strength)
{
    _body1 = body1;
    _body2 = body2;

    _strength = strength;

    _initialLength = (_body1->getPosition()-_body2->getPosition()).getNorm();
}

void U_2DSpring::applyForce()
{
    U_2DCoord v = _body1->getPosition()-_body2->getPosition();
    double dist = v.getNorm()+0.000001;
    double delta = _initialLength - dist;
    double f = 0.5*_strength*delta/dist;

    _body1->accelerate2D(U_2DCoord(f*v.x, f*v.y));
    _body2->accelerate2D(-U_2DCoord(f*v.x, f*v.y));
}
