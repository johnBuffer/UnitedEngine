#pragma once

#include <SFML/System/Clock.hpp>
#include <iostream>

#include "collision_solver.h"
#include "constraint_solver.h"

namespace up
{

using BodyPtr = fva::Handle<Body>;

class UnitedSolver
{
public:
	UnitedSolver() = default;
	UnitedSolver(const Vec2& dimension, uint32_t body_radius, const Vec2& gravity = Vec2(0.0f,0.0f)) :
		_collision_solver(dimension, body_radius, gravity),
		_dimension(dimension)
	{}

	BodyPtr addBody(const Vec2& position, float radius = 0.0f)
	{
		return _collision_solver.addBody(position, radius);
	}

	ConstraintPtr addConstraint(BodyPtr b1, BodyPtr b2)
	{
		return _constraint_solver.addConstraint(b1, b2);
	}

	AnchorPtr addAnchor(BodyPtr b, float max_length)
	{
		return _constraint_solver.addAnchor(b, max_length);
	}

	void update(float dt)
	{
		_collision_solver.update(dt);

		for (int i(2); i--;)
			_constraint_solver.update(dt);
	}

	const fva::SwapArray<Body>& bodies() const
	{
		return _collision_solver.bodies();
	}

	const Vec2& dimension() const
	{
		return _dimension;
	}

	float defaultBodyRadius() const
	{
		return _collision_solver.defaultBodyRadius();
	}

	void applyExplosion(const Vec2& position, float force)
	{
		_collision_solver.applyExplosion(position, force);
	}

	float physicsUpdateTime() const
	{
		return _collision_solver._up_time;
	}

private:
	const Vec2 _dimension;
	CollisionSolver _collision_solver;
	ConstraintSolver _constraint_solver;
};

}