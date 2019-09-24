#pragma once

#include "vec2.hpp"
#include <algorithm>
#include <iostream>

namespace up
{

class Body
{
public:
	Body() = default;
	Body(const Vec2& pos, float radius_) :
		m_position(pos),
		m_old_position(pos),
		m_acceleration(),
		pressure(0.0f),
		radius(radius_),
		m_moving(1),
		inertia(1.0f),
		debug(false),
		debug_collision(false)
	{}
	
	Body& operator=(const Body& b)
	{
		m_position     = b.m_position;
		m_old_position = b.m_old_position;
		m_acceleration = b.m_acceleration;
		pressure     = b.pressure;
		radius       = b.radius;

		return *this;
	}

	void update(float dt)
	{
		const Vec2 v(velocity());
		inertia = 1.0f + 0.1f * inertia + pressure / (v.length() + 1.0f);

		// Air friction
		m_acceleration -= v * 32.0f;

		// This prevent from too much compression
		const float anti_pressure_factor(std::pow(1.0f / inertia, 2));

		// Verlet integration
		m_old_position = m_position;
		m_position += float(m_moving) * (v + m_acceleration * anti_pressure_factor * dt * dt);

		// Reset temporary values
		m_acceleration = {};
		pressure = 0.0f;
	}

	const Vec2& position() const
	{
		return m_position;
	}

	const Vec2& oldPosition() const
	{
		return m_old_position;
	}

	const Vec2& acceleration() const
	{
		return m_acceleration;
	}

	Vec2 velocity() const
	{
		return m_position - m_old_position;
	}

	void setPosition(const Vec2& position)
	{
		m_position = position;
	}

	void move(const Vec2& delta)
	{
		const Vec2 d(m_moving * delta);
		m_position += d;
	}

	void moveOld(const Vec2& delta)
	{
		m_old_position += delta;
	}

	void accelerate(const Vec2& acceleration)
	{
		m_acceleration += acceleration;
	}

	void addPressure(float pressure_)
	{
		pressure += pressure_;
	}

	void stop()
	{
		m_old_position = m_position;
	}

	bool moving() const
	{
		return m_moving;
	}

	void moving(bool b)
	{
		m_moving = b;
	}

	void setVelocity(const Vec2& v)
	{
		moveOld(-1.0f*v);
	}

	float radius;
	float pressure;
	float inertia;

	bool done;
	bool debug;
	bool debug_collision;

private:
	Vec2 m_position;
	Vec2 m_old_position;
	Vec2 m_acceleration;

	uint8_t m_moving;
};

}




