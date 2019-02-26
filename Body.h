#pragma once

#include "vec2.h"

namespace up
{

class Body
{
public:
	Body() = default;
	Body(const Vec2& pos, float radius) :
		_position(pos),
		_old_position(pos),
		_acceleration(),
		_pressure(0.0f),
		_radius(radius),
		_moving(1)
	{}
	
	Body& operator=(const Body& b)
	{
		_position     = b._position;
		_old_position = b._old_position;
		_acceleration = b._acceleration;
		_pressure     = b._pressure;
		_radius       = b._radius;

		return *this;
	}

	void update(float dt)
	{
		// Air friction
		_acceleration += velocity() * -25.f;

		// This prevent from too much compression
		float anti_pressure_factor = 1.0f / (1.0f + _pressure);

		// Verlet integration
		Vec2 new_pos = _position + _moving*((_position - _old_position) + (_acceleration * anti_pressure_factor) * dt * dt);
		_old_position = _position;
		_position = new_pos;

		// Reset temporary values
		_acceleration = {};
		_pressure = 0.0f;
	}

	const Vec2& position() const
	{
		return _position;
	}

	const Vec2& acceleration() const
	{
		return _acceleration;
	}

	Vec2 velocity() const
	{
		return _position - _old_position;
	}

	void move(const Vec2& delta)
	{
		Vec2 d = _moving * delta;
		_position += d;
		moveOld(d * (1.0f / (_pressure + 1.0f)));
	}

	void moveHard(const Vec2& delta)
	{
		_position += _moving*delta;
	}

	void moveOld(const Vec2& delta)
	{
		_old_position += delta;
	}

	void accelerate(const Vec2& acceleration)
	{
		_acceleration += acceleration;
	}

	void addPressure(float pressure)
	{
		_pressure += pressure;
	}

	float radius() const
	{
		return _radius;
	}

	float mass() const
	{
		return 1.0f + _pressure;
	}

	void stop()
	{
		_old_position = _position;
	}

	bool moving() const
	{
		return _moving;
	}

	void moving(bool b)
	{
		_moving = b;
	}

private:
	Vec2 _position;
	Vec2 _old_position;
	Vec2 _acceleration;

	float _pressure;
	float _radius;
	uint8_t _moving;
};

}
