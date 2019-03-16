#pragma once

#include "Body.h"
#include "FastVersatileArray.hpp"

namespace up
{

using BodyPtr = fva::Handle<Body>;

class Constraint
{
public:
	Constraint():
		_body1(),
		_body2(),
		_length(0.0f),
		_broken(false)
	{}

	Constraint(fva::Handle<Body>& b1, fva::Handle<Body>& b2, float length = 0.0f) :
		_body1(b1),
		_body2(b2),
		_length(length),
		_broken(false),
		_resistance(1000.0f)
	{
		if (!_length)
		{
			Vec2 dir = b1->position() - b2->position();
			_length = dir.length();
		}
	}

	void resitance(float r)
	{
		_resistance = r;
	}

	void update(float dt)
	{
		if (_broken)
			return;

		Vec2 dir = _body1->position() - _body2->position();
		float current_length = dir.length();
		float delta_length = 0.5f*(_length - current_length);
		
		if (std::fabs(2.0f*delta_length) > _resistance)
			_broken = true;
		else
		{
			dir.normalize();
			_body1->moveHard( delta_length*dir);
			_body2->moveHard((-delta_length)*dir);
		}
	}

	const Vec2& position1() const
	{
		return _body1->position();
	}

	const Vec2& position2() const
	{
		return _body2->position();
	}

	bool broken() const
	{
		return _broken;
	}

private:
	fva::Handle<Body> _body1, _body2;
	float _length;
	bool _broken;
	float _resistance;
};


class Anchor
{
public:
	Anchor() = default;
	Anchor(BodyPtr body, float max_length = 1000.0f) :
		_body(body),
		_attach_point(body->position()),
		_max_length(max_length),
		_broken(false)
	{}

	void update(float dt)
	{
		if (_broken)
			return;

		Vec2 position = _body->position();
		Vec2 dir = _attach_point - position;

		if (dir.length() > _max_length)
			_broken = true;
		else
		{
			_body->moveHard(0.5f*dir);
		}
	}

	bool broken() const
	{
		return _broken;
	}

private:
	BodyPtr _body;
	Vec2 _attach_point;
	float _max_length;
	bool _broken;
};


using ConstraintPtr = fva::Handle<Constraint>;
using AnchorPtr = fva::Handle<Anchor>;

class ConstraintSolver
{
public:
	ConstraintSolver() = default;

	ConstraintPtr addConstraint(BodyPtr b1, BodyPtr b2)
	{
		return _constraints.add(b1, b2);
	}

	AnchorPtr addAnchor(BodyPtr b, float max_length)
	{
		return _anchors.add(b, max_length);
	}

	void update(float dt)
	{
		for (Constraint& c : _constraints)
		{
			c.update(dt);
		}

		for (Anchor& a : _anchors)
		{
			a.update(dt);
		}
	}

	const fva::SwapArray<Constraint>& constraints() const
	{
		return _constraints;
	}

private:
	fva::SwapArray<Constraint> _constraints;
	fva::SwapArray<Anchor> _anchors;
};




}