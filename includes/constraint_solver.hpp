#pragma once

#include "physic_body.hpp"
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
		m_length(0.0f),
		m_broken(false),
		m_strength(1.0f)
	{}

	Constraint(fva::Handle<Body>& b1, fva::Handle<Body>& b2, float length = 0.0f, float resistance = 1000.0f) :
		body1(b1),
		body2(b2),
		m_length(length),
		m_broken(false),
		m_resistance(resistance),
		m_strength(1.0f)
	{
		if (!m_length)
		{
			Vec2 dir = b1->position() - b2->position();
			m_length = dir.length();
		}
	}

	void resitance(float r)
	{
		m_resistance = r;
	}

	float length() const
	{
		return m_length;
	}

	void strength(float s)
	{
		m_strength = s;
	}

	void length(float l)
	{
		m_length = l;
	}

	void update(float dt)
	{
		if (m_broken)
			return;

		Vec2 dir = body1->position() - body2->position();
		float current_length = dir.length();
		float delta_length = 0.5f*(m_length - current_length);
		
		if (std::fabs(2.0f*delta_length) > m_resistance) {
			m_broken = true;
		}
		else
		{
			const float force(m_strength*delta_length);

			dir.normalize();
			body1->move((+force)*dir);
			body2->move((-force)*dir);

			/*const int32_t p(4);
			body1->accelerate((+std::pow(force, p))*dir);
			body2->accelerate((-std::pow(force, p))*dir);*/
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
		return m_broken;
	}

	fva::Handle<Body> body1, body2;
private:
	float m_length;
	bool m_broken;
	float m_resistance;
	float m_strength;
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
		m_body(body),
		m_attach_point(body->position()),
		m_max_length(max_length),
		m_broken(false)
	{}

	void update(float dt)
	{
		if (m_broken)
			return;

		Vec2 position = m_body->position();
		Vec2 dir = m_attach_point - position;

		if (dir.length() > m_max_length) {
			m_broken = true;
		}
		else
		{
			m_body->move(0.5f*dir);
		}
	}

	bool broken() const
	{
		return m_broken;
	}

private:
	BodyPtr m_body;
	Vec2 m_attach_point;
	float m_max_length;
	bool m_broken;
};


using ConstraintPtr = fva::Handle<Constraint>;
using MusclePtr = fva::Handle<Muscle>;
using AnchorPtr = fva::Handle<Anchor>;

class ConstraintSolver
{
public:
	using ConstraintsRef = fva::SwapArray<Constraint>&;
	using AnchorsRef = fva::SwapArray<Anchor>&;
	
	ConstraintSolver() = default;

	void update(ConstraintsRef constraints, AnchorsRef anchors, float dt)
	{
		const uint32_t precision(16);
		for (uint32_t i(precision); i--;)
		for (Constraint& c : constraints)
		{
			c.update(dt);
		}

		for (Anchor& a : anchors)
		{
			a.update(dt);
		}
	}
};




}