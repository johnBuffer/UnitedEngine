#ifndef REPULSION_H_INCLUDED
#define REPULSION_H_INCLUDED

#include "U_2DBody.h"

class U_2DAttraction
{
public:
    U_2DAttraction();
    U_2DAttraction(U_2DBody* b1, U_2DBody* b2, double strength);
    void applyAttraction();

private:
    U_2DBody *m_body1, *m_body2;
    double m_strength;

};

#endif // REPULSION_H_INCLUDED
