#pragma once

#include "fast_array.hpp"
#include "physic_body.hpp"

namespace up
{

using BodyPtr = fva::Handle<Body>;

class Constraint
{
public:
	Constraint() :
		body1(),
		body2(),
		m_length(0.0f),
		m_broken(false),
		m_strength(1.0f)
	{}

	Constraint(BodyPtr& b1, BodyPtr& b2, float length = 0.0f, float resistance = 1000.0f) :
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
		} else {
			const float force(m_strength*delta_length);
			dir.normalize();
			body1->moveHard((+force)*dir);
			body2->moveHard((-force)*dir);
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


}


