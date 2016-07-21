#ifndef U_2DSPRING_H_INCLUDED
#define U_2DSPRING_H_INCLUDED

#include "U_2DBody.h"

class U_2DSpring
{
public:
    U_2DSpring(U_2DBody* body1, U_2DBody* body2, double strength);
    void applyForce();

    U_2DCoord getBody1Pos() {return _body1->getPosition();};
    U_2DCoord getBody2Pos() {return _body2->getPosition();};

    void strengthFactor(double f) {_strength *= f;}

private:
    U_2DBody *_body1, *_body2;
    double _initialLength, _strength;

};

#endif // U_2DSPRING_H_INCLUDED
