#ifndef U_2DCOLISION_H_INCLUDED
#define U_2DCOLISION_H_INCLUDED

#include "U_2DBody.h"
#include "U_2DConstraint.h"
#include "U_2DSpring.h"
#include "U_2DAttraction.h"
#include "U_2DRigidBody.h"

#include<map>
#include<vector>
#include<cmath>
#include<list>

class U_2DCollisionManager
{
public:
    U_2DCollisionManager();
    U_2DCollisionManager(double timeStep, double bodyRadius, U_2DCoord mapSize);

    U_2DBody*       getBodyAt(int i);
    U_2DBody*       getBodyAt(U_2DCoord coord);
    U_2DRigidBody*  getRigidBodyAt(int i);
    U_2DSpring*     getSpringAt(int i)          {return m_springs[i];}

    double      getBodyRadius()       const {return m_bodySize/2;};
    int         getBodiesCount()      const {return m_bodies.size();}
    int         getConstraintsCount() const {return m_constraints.size();}
    int         getRigidBodiesCount() const {return m_rigidBodies.size();}
    int         getSpringsCount()     const {return m_springs.size();}
    U_2DCoord   getMapSize()          const {return m_mapSize;};

    const std::list<U_2DConstraint*> getConstraints() const {return m_constraints;}

    void addRigidBody(U_2DRigidBody &rigidBody, U_2DCoord position);
    void setGravity(U_2DCoord gravityVec) {m_gravity = gravityVec;};
    void setPrecision(unsigned int iterationCount) {m_iterationCount = iterationCount;};
    void addAttraction(U_2DBody* b1, U_2DBody* b2, double strength);
    void applyExplosion(U_2DCoord explosionCoord, double force);
    void killBody(U_2DBody* body);
    void update();

    U_2DConstraint* addConstraint(U_2DBody* b1, U_2DBody* b2, double length=0);
    U_2DConstraint* addConstraint(U_2DBody* b1, U_2DBody* b2, double maxElongationFactor, double length=0);
    U_2DSpring*     addSpring(U_2DBody* b1, U_2DBody* b2, double strength);
    U_2DBody*       addBody(U_2DCoord coord, double mass);
    U_2DBody*       addBody(U_2DCoord coord, double radius, double mass);
    void            addBody(U_2DBody* body)    {m_bodies.push_back(body);};

private:
    double    m_timeStep, m_bodySize;
    int       m_nCollisionChecked, m_iterationCount;
    U_2DCoord m_mapSize, m_gravity;

    std::vector<U_2DBody*>       m_bodies;
    std::vector<U_2DRigidBody*>  m_rigidBodies;
    std::list<U_2DConstraint*>   m_constraints;
    std::vector<U_2DAttraction*> m_attractions;
    std::vector<U_2DSpring*>     m_springs;

    std::map<int, std::vector<U_2DBody*> > m_grid;

    void addBodyToGrid(U_2DBody* body);
    long convertPosToHash(int x, int y) const;
    void applyGravity();
    void solveGridCollisions(std::vector<U_2DBody*> &bodies);
    void solveCollisions();
    void solveAttractions();
    void solveBoundCollisions(U_2DBody* body);
    void solveConstraints();
    void solveSprings();
};


#endif // U_2DCOLISION_H_INCLUDED
