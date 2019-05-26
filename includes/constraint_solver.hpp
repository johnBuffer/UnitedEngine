#pragma once

#include "physic_body.hpp"
#include "fast_array.hpp"
#include "constraint.hpp"
#include "segment.hpp"

namespace up
{

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

