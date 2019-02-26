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

	BodyPtr addBody(const Vec2& position)
	{
		return _collision_solver.addBody(position);
	}

	ConstraintPtr addConstraint(BodyPtr b1, BodyPtr b2)
	{
		return _constraint_solver.addConstraint(b1, b2);
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

private:
	const Vec2 _dimension;
	CollisionSolver _collision_solver;
	ConstraintSolver _constraint_solver;
};

}