#pragma once

#include "constraint_solver.h"

namespace up
{

class Muscle
{
public:
	Muscle(BodyPtr b1, BodyPtr b2) :
		m_constraint(b1, b2)
	{
		m_idle_length = m_constraint.length();
	}

	void update(float dt)
	{
		m_constraint.update(dt);
	}

	void contract(float ratio)
	{
		m_constraint.length(m_idle_length * (1.0f - ratio));
	}

	Constraint& constraint()
	{
		return m_constraint;
	}

private:
	Constraint m_constraint;
	float m_idle_length;
};

}
