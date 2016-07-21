#ifndef U_2DCONSTRAINT_H_INCLUDED
#define U_2DCONSTRAINT_H_INCLUDED

#include "U_2DBody.h"
#include <cmath>

class U_2DConstraint
{
public:
    U_2DConstraint(U_2DBody* b1, U_2DBody* b2);
    U_2DConstraint(U_2DBody* b1, U_2DBody* b2, double maxElongationFactor, double length=0);
    void applyConstraint();

    U_2DCoord getBody1Pos() {return m_body1->getPosition();};
    U_2DCoord getBody2Pos() {return m_body2->getPosition();};

    void setOnlyTension(bool tension) {_onlyTension = tension;}

    bool isBroken() const {return m_broken;};

private:
    U_2DBody *m_body1, *m_body2;
    double m_length, _maxElongationFactor;
    bool m_broken;
    bool _onlyTension;

};

#endif // U_2DCONSTRAINT_H_INCLUDED
