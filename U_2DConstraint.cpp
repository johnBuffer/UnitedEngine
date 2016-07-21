#include "U_2DConstraint.h"

U_2DConstraint::U_2DConstraint(U_2DBody* b1, U_2DBody* b2)
{
    m_body1 = b1;
    m_body2 = b2;

    m_length = U_2DCoord(m_body2->getPosition(), m_body1->getPosition()).getNorm();
    m_broken = false;
    _maxElongationFactor = 0;
    _onlyTension = false;
}

U_2DConstraint::U_2DConstraint(U_2DBody* b1, U_2DBody* b2, double maxElongationFactor, double length)
{
    m_body1 = b1;
    m_body2 = b2;

    m_length = length;
    if (length == 0)
        m_length = U_2DCoord(m_body2->getPosition(), m_body1->getPosition()).getNorm();

    m_broken = false;
    _maxElongationFactor = maxElongationFactor;
    _onlyTension = false;
}


void U_2DConstraint::applyConstraint()
{
    double vx = m_body1->getPosition().x-m_body2->getPosition().x;
    double vy = m_body1->getPosition().y-m_body2->getPosition().y;

    double mass1 = m_body1->getMass();
    double mass2 = m_body2->getMass();

    double coeffMass2 = 1;
    double coeffMass1 = 1;

    if (mass1 != 0 && mass2 != 0)
    {
        coeffMass2 = mass1/(mass1+mass2);
        coeffMass1 = mass2/(mass1+mass2);
    }

    double dist = sqrt(vx*vx+vy*vy);

    if (_maxElongationFactor != 0 && dist > m_length*_maxElongationFactor)
    {
        m_broken = true;
    }
    else
    {
        double delta = dist-m_length;
        double constraintForce = (delta)*0.5/dist;

        vx *= constraintForce;
        vy *= constraintForce;

        if ((_onlyTension && delta>0) || !_onlyTension)
        {
            if (delta < 0)
            {
                coeffMass1 = 1;
                coeffMass2 = 1;
            }

            m_body2->move2D(U_2DCoord(coeffMass2*vx, coeffMass2*vy));
            m_body1->move2D(U_2DCoord(-coeffMass1*vx, -coeffMass1*vy));
        }
    }
}
