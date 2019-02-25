#pragma once

#include "vec2.h"

class Body
{
public:
	Body() = default;
	Body(const Vec2& pos, float radius, uint32_t id) :
		_position(pos),
		_old_position(pos),
		_acceleration(),
		_pressure(0.0f),
		_radius(radius),
		_id(id)
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
		_acceleration += velocity() * -25.f;
		float anti_pressure_factor = 1.0f / (1.0f + _pressure);
		Vec2 new_pos = _position + (_position - _old_position) + (_acceleration * anti_pressure_factor) * dt * dt;
		_old_position = _position;
		_position = new_pos;

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
		_position += delta;
		_old_position += delta * (10.0f / (_pressure + 10.0f));
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

	uint32_t id() const
	{
		return _id;
	}

private:
	Vec2 _position;
	Vec2 _old_position;
	Vec2 _acceleration;

	float _pressure;
	float _radius;
	uint32_t _id;
};