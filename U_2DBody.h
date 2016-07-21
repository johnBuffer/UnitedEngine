#ifndef U_2DBODY_H_INCLUDED
#define U_2DBODY_H_INCLUDED

#include "U_2DCoord.h"

class U_2DBody
{
public:
    U_2DBody();
    U_2DBody(U_2DCoord position, double mass, bool included=false);
    U_2DBody(U_2DCoord position, double radius, double mass, bool included=false);
    void move2D(U_2DCoord vec);
    void moveLastPos2D(U_2DCoord vec);
    void accelerate2D(U_2DCoord acc);
    void updatePosition(double timestep);
    void setX(double x) {m_position.x = x;};// m_lastPosition.x = x;};
    void setY(double y) {m_position.y = y;};// m_lastPosition.y = y;};
	void stop() {m_lastPosition = m_position;};
	void setStatic(bool isStatic) {m_static = isStatic;}
	void setIncluded(bool included) {m_included = included;}
	void setPressure(double pressure) {m_pressure = pressure;}
	void addPressure(double p) {m_pressure += p; if (m_pressure>255) m_pressure=255;}
	void setPenetrationVelocity(double v) {m_penetrationVelocity = v;}

    U_2DCoord getPosition() const {return m_position;};
    U_2DCoord getVelocity() const;
    bool isStatic() const {return m_static;};
    bool isIncluded() const {return m_included;}
    double getMass() const {return m_mass;};
    double getPressure() const {return m_pressure;};
    double getPenetrationVelocity() const {return m_penetrationVelocity;};
    double getRadius() const {return m_radius;}

private:
    void init(U_2DCoord position, double radius, double mass, bool included);
    U_2DCoord m_position, m_lastPosition, m_acceleration;
    double m_radius, m_mass, m_pressure, m_penetrationVelocity;
    bool m_static, m_included, m_gridChange;
};

#endif // U_2DBODY_H_INCLUDED
