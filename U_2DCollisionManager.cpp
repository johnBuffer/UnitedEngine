#include "U_2DCollisionManager.h"
#include <iostream>

#include <SFML/Graphics.hpp>

U_2DCollisionManager::U_2DCollisionManager()
{
    m_timeStep = 1;
    m_bodySize = 1;
    m_mapSize = U_2DCoord();

    m_iterationCount = 0;
    m_currentIteration = 0;

    m_gravity = U_2DCoord();
}

U_2DCollisionManager::U_2DCollisionManager(double timeStep, double bodyRadius, U_2DCoord mapSize)
{
    m_timeStep = timeStep;
    m_bodySize = bodyRadius*2;
    m_mapSize = mapSize;

    m_iterationCount = 2;
    m_currentIteration = 0;

    m_gravity = U_2DCoord(0, 100);
}

U_2DBody* U_2DCollisionManager::addBody(U_2DCoord coord, double mass)
{
    U_2DBody* newBody = new U_2DBody(coord, m_bodySize/2, mass);
    m_bodies.push_back(newBody);

    return newBody;
}

U_2DBody* U_2DCollisionManager::addBody(U_2DCoord coord, double radius, double mass)
{
    U_2DBody* newBody = new U_2DBody(coord, radius, mass);
    m_bodies.push_back(newBody);

    return newBody;
}

void U_2DCollisionManager::addRigidBody(U_2DRigidBody &rigidBodyOriginal, U_2DCoord position, double strength)
{
    U_2DBody *b1(NULL), *b2(NULL), *b3(NULL), *b4(NULL);
    U_2DBody *currentBody(NULL);
    int n_bodies = m_bodies.size();
    int n_included = 0;
    int width = rigidBodyOriginal.getWidth();
    int height = rigidBodyOriginal.getHeight();

    U_2DRigidBody* rigidBody = new U_2DRigidBody(width, height);

    for (int x=0; x<width; ++x)
    {
        for (int y=0; y<height; ++y)
        {
            U_2DCoord pos(x*m_bodySize+position.x, y*m_bodySize+position.y);
            currentBody = addBody(pos, 1);
            currentBody->setIncluded(true);
            if (y == 0)
            {
                if (x==0)
                    b1 = currentBody;
                else if (x==rigidBody->getWidth()-1)
                    b2 = currentBody;
            }
            n_included++;
        }
        if (x == 0)
            b4 = currentBody;
    }
    b3 = currentBody;

    int firstIndice = m_bodies.size()-1-width*height;
    for (int i(0); i<width; ++i)
        m_bodies[firstIndice+(i+1)*height]->setStatic(true);

    rigidBody->setBoundsBodies(b1, b2, b3, b4);
    m_rigidBodies.push_back(rigidBody);

    for (int y=1; y<height; ++y)
    {
        int i = n_bodies+y;
        m_constraints.push_back(new U_2DConstraint(m_bodies[i], m_bodies[i-1], strength));
    }

    for (int x=1; x<width; ++x)
    {
        int i = n_bodies+x*height;
        m_constraints.push_back(new U_2DConstraint(m_bodies[i], m_bodies[i-height], strength));
    }

    for (int x=1; x<width; ++x)
    {
        for (int y=1; y<height; ++y)
        {
            int i1 = n_bodies+x*height+y;
            int i2 = n_bodies+(x-1)*height+y;
            int i3 = n_bodies+(x-1)*height+(y-1);
            int i4 = n_bodies+x*height+(y-1);

            m_constraints.push_back(new U_2DConstraint(m_bodies[i1], m_bodies[i2], strength));
            m_constraints.push_back(new U_2DConstraint(m_bodies[i1], m_bodies[i3], strength));
            m_constraints.push_back(new U_2DConstraint(m_bodies[i1], m_bodies[i4], strength));
        }
    }

    for (int x=0; x<width-1; ++x)
    {
        for (int y=0; y<height-1; ++y)
        {
            int i1 = n_bodies+x*height+y;
            int i2 = n_bodies+(x+1)*height+y;
            int i3 = n_bodies+(x+1)*height+(y+1);
            int i4 = n_bodies+x*height+(y+1);

            m_constraints.push_back(new U_2DConstraint(m_bodies[i1], m_bodies[i2], strength));
            m_constraints.push_back(new U_2DConstraint(m_bodies[i1], m_bodies[i3], strength));
            m_constraints.push_back(new U_2DConstraint(m_bodies[i1], m_bodies[i4], strength));
        }
    }
}

U_2DBody* U_2DCollisionManager::getBodyAt(int i)
{
    return m_bodies[i];
}

U_2DRigidBody* U_2DCollisionManager::getRigidBodyAt(int i)
{
    return m_rigidBodies[i];
}

long U_2DCollisionManager::convertPosToHash(int x, int y) const
{
    return x+y*1000000000L;
}

void U_2DCollisionManager::addBodyToGrid(U_2DBody* body)
{
    int bodyX = body->getPosition().x;
    int bodyY = body->getPosition().y;
    int caseSize = m_bodySize;
    int gridX = bodyX/caseSize;
    int gridY = bodyY/caseSize;
    int midGrid = caseSize/2;

    long newHash = convertPosToHash(gridX, gridY);
    /*if (body->lastHash != newHash)
    {
        m_newHash++;
        body->lastHash = newHash;
    }*/

    m_grid[newHash].add(body);

    if (bodyX%caseSize > midGrid)
    {
        m_grid[convertPosToHash(gridX+1, gridY)].add(body);
        if (bodyY%caseSize > midGrid)
        {
            m_grid[convertPosToHash(gridX, gridY+1)].add(body);
            m_grid[convertPosToHash(gridX+1, gridY+1)].add(body);
        }
        else
        {
            m_grid[convertPosToHash(gridX, gridY-1)].add(body);
            m_grid[convertPosToHash(gridX+1, gridY-1)].add(body);
        }
    }
    else
    {
        m_grid[convertPosToHash(gridX-1, gridY)].add(body);
        if (bodyY%caseSize > midGrid)
        {
            m_grid[convertPosToHash(gridX-1, gridY+1)].add(body);
            m_grid[convertPosToHash(gridX, gridY+1)].add(body);
        }
        else
        {
            m_grid[convertPosToHash(gridX-1, gridY-1)].add(body);
            m_grid[convertPosToHash(gridX, gridY-1)].add(body);
        }
    }
}

void U_2DCollisionManager::applyGravity()
{
    for (auto b : m_bodies)
    {
        b->accelerate2D(m_gravity);
    }
}

void U_2DCollisionManager::solveBoundCollisions(U_2DBody* body)
{
    int maxX = m_mapSize.x;
    int maxY = m_mapSize.y;
    int bodyX = body->getPosition().x;
    int bodyY = body->getPosition().y;

    int radius = body->getRadius();

    if (bodyY+radius+1 > maxY)
    {
        body->setY(maxY-radius);
    }
    if (bodyX+radius+1 > maxX)
    {
        body->setX(maxX-radius);
    }

    if (bodyY-radius-1 < 0)
    {
        double delta = -bodyY+radius;
        body->move2D(U_2DCoord(0, delta));
    }

    if (bodyX-radius < 0)
    {
        body->setX(radius);
    }
}

void U_2DCollisionManager::solveGridCollisions(GridCell& cell)
{
    double coeff = 1.0;

    int n_bodies = cell._maxIndex;
    U_2DBody** bodies = cell._bodies.data();

    for (int i(0); i<n_bodies; ++i)
    {
        U_2DBody* currentBody = bodies[i];
        double currentX = currentBody->getPosition().x;
        double currentY = currentBody->getPosition().y;
        double currentRadius = currentBody->getRadius();

        for (int k(i+1); k<n_bodies; k++)
        {
            U_2DBody* collider = bodies[k];

            double colliderRadius = collider->getRadius();
            double minDist = currentRadius+colliderRadius;

            double vx, vy, dist2;

            vx = currentX-collider->getPosition().x;
            vy = currentY-collider->getPosition().y;

            dist2 = vx*vx+vy*vy;

            if (dist2 && dist2 < minDist*minDist)
            {
                double dist = sqrt(dist2);
                double deltaDist = minDist-dist;

                double responseVelocity = std::min(0.5*coeff*deltaDist/(dist), m_bodySize*0.5*m_timeStep);

                vx *= responseVelocity;
                vy *= responseVelocity;

                currentBody->move2D(U_2DCoord( vx,  vy));
                collider   ->move2D(U_2DCoord(-vx, -vy));

                currentBody->addPressure(deltaDist*0.5);
                collider   ->addPressure(deltaDist*0.5);
            }
        }
    }
}

void U_2DCollisionManager::solveCollisions()
{
    for(auto &elem : m_grid) solveGridCollisions(elem.second);
    for (auto &body : m_bodies) solveBoundCollisions(body);
}

void U_2DCollisionManager::update()
{
    m_currentIteration++;

    m_nCollisionChecked = 0;
    m_newHash = 0;
    applyGravity();

    sf::Clock c1, c2;

    c1.restart();
    for (auto &elem : m_grid) elem.second.reset();
    for (auto &body : m_bodies) addBodyToGrid(body);
    std::cout << "parse time " << c2.getElapsedTime().asMilliseconds() << " ";// << "ms for " << int(100*m_newHash/double(m_bodies.size())) << "%" << std::endl;

    c2.restart();
    for (int i(0); i<m_iterationCount; ++i)
    {
        solveConstraints();
        solveCollisions();
        solveAttractions();
        solveSprings();
    }

    for (auto& body : m_bodies) body->updatePosition(m_timeStep);

    std::cout << "phys time " << c2.getElapsedTime().asMilliseconds() << " ms" << std::endl;
}

void U_2DCollisionManager::solveConstraints()
{
    for (auto& ctr : m_constraints)
    {
        ctr->applyConstraint();
    }

    m_constraints.remove_if([](U_2DConstraint*& c) {return c->isBroken();});
}

U_2DConstraint* U_2DCollisionManager::addConstraint(U_2DBody* b1, U_2DBody* b2, double length)
{
    U_2DConstraint* newConstraint = NULL;
    if (length > 0)
        newConstraint = new U_2DConstraint(b1, b2, length);
    else
        newConstraint = new U_2DConstraint(b1, b2);

    m_constraints.push_back(newConstraint);

    return newConstraint;
}

U_2DConstraint* U_2DCollisionManager::addConstraint(U_2DBody* b1, U_2DBody* b2, double maxElongationFactor, double length)
{
    U_2DConstraint* newConstraint = new U_2DConstraint(b1, b2, maxElongationFactor, length);
    m_constraints.push_back(newConstraint);
    return newConstraint;
}

void U_2DCollisionManager::addAttraction(U_2DBody* b1, U_2DBody* b2, double strength)
{
    m_attractions.push_back(new U_2DAttraction(b1, b2, strength));
}

U_2DSpring* U_2DCollisionManager::addSpring(U_2DBody* b1, U_2DBody* b2, double strength)
{
    U_2DSpring* newSpring = new U_2DSpring(b1, b2, strength);
    m_springs.push_back(newSpring);

    return newSpring;
}

void U_2DCollisionManager::solveAttractions()
{
    int n_attractions = m_attractions.size();
    for (int i=0; i<n_attractions; ++i) m_attractions[i]->applyAttraction();
}

void U_2DCollisionManager::solveSprings()
{
    for (auto& spring : m_springs)
        spring->applyForce();
}

void U_2DCollisionManager::applyExplosion(U_2DCoord explosionCoord, double force)
{
    for (U_2DBody* body : m_bodies)
    {
        U_2DCoord v(explosionCoord, body->getPosition());

        double dist = v.getNorm();
        double f = (force/m_timeStep)/(dist*dist*dist);
        v.x *= f;
        v.y *= f;

        if (dist > 100)
            body->accelerate2D(v);
    }
}

U_2DBody* U_2DCollisionManager::getBodyAt(U_2DCoord coord)
{
    for (U_2DBody* &b : m_bodies)
    {
        if ( (b->getPosition() - coord).getNorm() < m_bodySize )
        {
            return b;
        }
    }

    return NULL;
}

