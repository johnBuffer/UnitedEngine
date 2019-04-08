#pragma once

#include "Body.h"
#include "fast_array.hpp"

namespace up
{

using BodyPtr = fva::Handle<Body>;


class Constraint
{
public:
	Constraint():
		body1(),
		body2(),
		_length(0.0f),
		_broken(false),
		_strength(1.0f)
	{}

	Constraint(fva::Handle<Body>& b1, fva::Handle<Body>& b2, float length = 0.0f) :
		body1(b1),
		body2(b2),
		_length(length),
		_broken(false),
		_resistance(1000.0f),
		_strength(1.0f)
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

	float length() const
	{
		return _length;
	}

	void strength(float s)
	{
		_strength = s;
	}

	void length(float l)
	{
		_length = l;
	}

	void update(float dt)
	{
		if (_broken)
			return;

		Vec2 dir = body1->position() - body2->position();
		float current_length = dir.length();
		float delta_length = _strength*0.5f*(_length - current_length);
		
		if (std::fabs(2.0f*delta_length) > _resistance)
			_broken = true;
		else
		{
			dir.normalize();
			body1->moveHard( delta_length*dir);
			body2->moveHard((-delta_length)*dir);
		}
	}

	const Vec2& position1() const
	{
		return body1->position();
	}

	const Vec2& position2() const
	{
		return body2->position();
	}

	bool broken() const
	{
		return _broken;
	}

	fva::Handle<Body> body1, body2;
private:
	float _length;
	bool _broken;
	float _resistance;
	float _strength;
};


class Muscle
{
public:
	Muscle(BodyPtr b1, BodyPtr b2) :
		m_constraint(b1, b2),
		m_contraction(0.0f)
	{
		m_constraint.strength(0.075f);
		m_idle_length = m_constraint.length();
	}

	void update(float dt)
	{
		m_constraint.update(dt);
	}

	void contract(float ratio)
	{
		m_contraction = 1.0f - ratio;
		m_constraint.length(m_idle_length * m_contraction);
	}

	const Constraint& constraint() const
	{
		return m_constraint;
	}

	float contraction() const
	{
		return 1.0f - m_contraction;
	}

private:
	Constraint m_constraint;
	float m_idle_length;
	float m_contraction;
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
using MusclePtr = fva::Handle<Muscle>;
using AnchorPtr = fva::Handle<Anchor>;

class ConstraintSolver
{
public:
	ConstraintSolver() = default;

	ConstraintPtr addConstraint(BodyPtr b1, BodyPtr b2)
	{
		return _constraints.add(b1, b2);
	}

	MusclePtr addMuscle(BodyPtr b1, BodyPtr b2)
	{
		return _muscles.add(b1, b2);
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

		for (Muscle& m : _muscles)
		{
			m.update(dt);
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

	const fva::SwapArray<Muscle>& muscles() const
	{
		return _muscles;
	}

private:
	fva::SwapArray<Constraint> _constraints;
	fva::SwapArray<Muscle> _muscles;
	fva::SwapArray<Anchor> _anchors;
};




}