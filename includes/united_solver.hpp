#pragma once

#include <SFML/System/Clock.hpp>
#include <iostream>

#include "collision_solver.hpp"
#include "constraint_solver.hpp"

namespace up
{

using BodyPtr = fva::Handle<Body>;

class UnitedSolver
{
public:
	UnitedSolver() = default;

	UnitedSolver(const Vec2& dimension, float body_radius, const Vec2& gravity = Vec2(0.0f, 0.0f)) :
		m_collision_solver(dimension, body_radius, gravity),
		m_dimension(dimension)
	{}

	void update(float dt)
	{
		m_collision_solver.update(m_bodies, dt);
		m_constraint_solver.update(m_constraints, m_anchors, dt);
		for (Body& b : m_bodies) {
			b.update(dt);
		}
	}

	BodyPtr addBody(const Vec2& position)
	{
		return m_bodies.add(position, m_collision_solver.defaultBodyRadius());
	}

	BodyPtr addBody(const Vec2& position, float radius)
	{
		float r(std::min(radius, m_collision_solver.defaultBodyRadius()));
		return m_bodies.add(position, r);
	}

	ConstraintPtr addConstraint(BodyPtr& body1, BodyPtr& body2, float length = 0.0f, float resistance = 1000.0f)
	{
		return m_constraints.add(body1, body2, length, resistance);
	}

	const Vec2& getDimension() const
	{
		return m_dimension;
	}

	const fva::SwapArray<Body>& getBodies() const
	{
		return m_bodies;
	}

	const fva::SwapArray<Constraint>& getConstraints() const
	{
		return m_constraints;
	}

private:
	const Vec2 m_dimension;
	CollisionSolver m_collision_solver;
	ConstraintSolver m_constraint_solver;

	fva::SwapArray<Body> m_bodies;
	fva::SwapArray<Constraint> m_constraints;
	fva::SwapArray<Anchor> m_anchors;
};

}