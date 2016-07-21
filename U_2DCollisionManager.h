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
#include<mutex>

class U_2DCollisionManager
{
public:
    U_2DCollisionManager();
    U_2DCollisionManager(double timeStep, double bodyRadius, U_2DCoord mapSize);
    U_2DBody* getBodyAt(int i);
    U_2DBody* getBodyAt(U_2DCoord coord);
    U_2DRigidBody* getRigidBodyAt(int i);
    U_2DBody* addBody(U_2DCoord coord, double mass);
    U_2DBody* addBody(U_2DCoord coord, double radius, double mass);
    U_2DConstraint* getConstraintAt(int i) {return m_constraints[i];};

    U_2DSpring* getSpringAt(int i) {return m_springs[i];}
    void addBody(U_2DBody* body) {m_bodies.push_back(body);};
    double getBodyRadius() const {return m_bodySize/2;};
    int getBodiesCount() const  {return m_bodies.size();}
    int getConstraintsCount() const  {return m_constraints.size();}
    int getRigidBodiesCount() const  {return m_rigidBodies.size();}
    int getSpringsCount() const  {return m_springs.size();}

    U_2DCoord getMapSize() const {return m_mapSize;};

    void addRigidBody(U_2DRigidBody &rigidBody, U_2DCoord position);
    void setGravity(U_2DCoord gravityVec) {m_gravity = gravityVec;};
    void setPrecision(unsigned int iterationCount) {m_iterationCount = iterationCount;};
    U_2DConstraint* addConstraint(U_2DBody* b1, U_2DBody* b2, double length=0);
    U_2DConstraint* addConstraint(U_2DBody* b1, U_2DBody* b2, double maxElongationFactor, double length=0);
    U_2DSpring* addSpring(U_2DBody* b1, U_2DBody* b2, double strength);
    void addAttraction(U_2DBody* b1, U_2DBody* b2, double strength);
    void applyExplosion(U_2DCoord explosionCoord, double force);
    void killBody(U_2DBody* body);
    void update();

private:
    double m_timeStep, m_bodySize;
    int m_nCollisionChecked, m_iterationCount;
    U_2DCoord m_mapSize, m_gravity;
    std::vector<U_2DBody*> m_bodies;
    std::vector<U_2DRigidBody*> m_rigidBodies;

    std::map<int, std::vector<U_2DBody*> > m_grid;
    std::vector<U_2DConstraint*> m_constraints;
    std::vector<U_2DAttraction*> m_attractions;
    std::vector<U_2DSpring*> m_springs;

    void addBodyToGrid(U_2DBody* body);
    int convertPosToHash(int x, int y);
    void applyGravity();
    void solveGridCollisions(std::vector<U_2DBody*> &bodies);
    void solveCollisions();
    void solveAttractions();
    void solveBoundCollisions(U_2DBody* body);
    void solveConstraints();
    void solveSprings();
};


#endif // U_2DCOLISION_H_INCLUDED
