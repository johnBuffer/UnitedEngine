#pragma once

#include "Body.h"
#include "FastVersatileArray.hpp"

namespace up
{

class Constraint
{
public:
	Constraint():
		_body1(),
		_body2(),
		_length(0.0f)
	{}

	Constraint(fva::Handle<Body>& b1, fva::Handle<Body>& b2, float length = 0.0f) :
		_body1(b1),
		_body2(b2),
		_length(length)
	{
		if (!_length)
		{
			Vec2 dir = b1->position() - b2->position();
			_length = dir.length();
		}
	}

	void update(float dt)
	{
		Vec2 dir = _body1->position() - _body2->position();
		float current_length = dir.length();
		float delta_length = 0.5f*(_length - current_length);

		dir.normalize();

		_body1->moveHard(delta_length*dir);
		_body2->moveHard((-delta_length)*dir);
	}

private:
	fva::Handle<Body> _body1, _body2;
	float _length;
};

using ConstraintPtr = fva::Handle<Constraint>;

class ConstraintSolver
{
public:
	ConstraintSolver() = default;

	ConstraintPtr addConstraint(fva::Handle<Body>& b1, fva::Handle<Body>& b2)
	{
		return _constraints.add(b1, b2);
	}

	void update(float dt)
	{
		for (Constraint& c : _constraints)
		{
			c.update(dt);
		}
	}

private:
	fva::SwapArray<Constraint> _constraints;
};




}