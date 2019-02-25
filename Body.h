#pragma once

#include "vec2.h"

class Body
{
public:
	Body() = default;
	Body(const Vec2& pos, float radius, float mass=1.0) :
		_position(pos),
		_old_position(pos),
		_acceleration(),
		_radius(radius),
		_mass(mass)
	{}

	void update(float dt)
	{
		Vec2 new_pos = _position + (_position - _old_position) + _acceleration * dt * dt;
		_old_position = _position;
		_position = new_pos;

		_acceleration = {};
	}

	const Vec2& position() const
	{
		return _position;
	}

	Vec2 velocity() const
	{
		return _position - _old_position;
	}

	void move(const Vec2& delta)
	{
		_position += delta;
	}

	void accelerate(const Vec2& acceleration)
	{
		_acceleration += acceleration;
	}

	float radius() const
	{
		return _radius;
	}

	float mass() const
	{
		return _mass;
	}

private:
	Vec2 _position;
	Vec2 _old_position;
	Vec2 _acceleration;

	const float _radius;
	float _mass;
};