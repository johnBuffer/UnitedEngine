#pragma once

#include "vec2.h"
#include <algorithm>

namespace up
{

class Body
{
public:
	Body() = default;
	Body(const Vec2& pos, float radius) :
		m_position(pos),
		m_old_position(pos),
		m_acceleration(),
		m_pressure(0.0f),
		m_old_pressure(0.0f),
		m_radius(radius),
		m_moving(1),
		m_inertia(1.0f)
	{}
	
	Body& operator=(const Body& b)
	{
		m_position     = b.m_position;
		m_old_position = b.m_old_position;
		m_acceleration = b.m_acceleration;
		m_pressure     = b.m_pressure;
		m_radius       = b.m_radius;

		return *this;
	}

	void update(float dt)
	{
		Vec2 v = velocity();
		m_inertia = 0.1f * m_inertia + 1.0f + m_old_pressure / (v.length2() + 1.0f);

		// Air friction
		m_acceleration += v * -30.0f;

		// This prevent from too much compression
		float anti_pressure_factor = std::pow(1.0f / m_inertia, 3);

		// Verlet integration
		m_old_position = m_position;
		/*Vec2 delta_pos = _moving * v + (_acceleration * anti_pressure_factor) * dt * dt;
		if (delta_pos.length2() > _radius*_radius)
		{
			delta_pos.normalize();
			delta_pos = delta_pos * _radius;
		}*/

		m_position += v + (m_acceleration * anti_pressure_factor) * dt * dt;

		// Reset temporary values
		m_acceleration = {};

		m_old_pressure = std::min(m_pressure, m_radius);
		m_pressure = 0.0f;
	}

	const Vec2& position() const
	{
		return m_position;
	}

	const Vec2& acceleration() const
	{
		return m_acceleration;
	}

	Vec2 velocity() const
	{
		return m_position - m_old_position;
	}

	void move(const Vec2& delta)
	{
		const Vec2 d = m_moving * delta;
		m_position += d;
		m_old_position += 0.1f * d;
	}

	void moveHard(const Vec2& delta)
	{
		m_position += m_moving*delta;
	}

	void moveOld(const Vec2& delta)
	{
		m_old_position += delta;
	}

	void accelerate(const Vec2& acceleration)
	{
		m_acceleration += acceleration;
	}

	void addPressure(float pressure)
	{
		m_pressure += pressure;
	}

	float radius() const
	{
		return m_radius;
	}

	float mass() const
	{
		return m_inertia;
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

private:
	Vec2 m_position;
	Vec2 m_old_position;
	Vec2 m_acceleration;

	float m_radius;
	float m_pressure;
	float m_old_pressure;
	float m_inertia;
	uint8_t m_moving;
};

class UnitedSolver;

class FastCollider
{
public:
	FastCollider() = default;

	FastCollider(const Vec2& position, const Vec2& dir, uint32_t speed_factor) :
		_position(position),
		_direction(dir),
		_speed(speed_factor)
	{

	}

	virtual void onHit(UnitedSolver* solver) = 0;

private:
	Vec2 _position;
	Vec2 _direction;
	uint32_t _speed;
};


}