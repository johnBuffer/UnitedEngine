#pragma once

#include "FastVersatileArray.hpp"
#include "Body.h"
#include <vector>

class UnitedSolver
{
public:
	UnitedSolver() = default;
	UnitedSolver(const Vec2& dimension, const Vec2& gravity = Vec2(0.0f,0.0f)) :
		_dimension(dimension),
		_gravity(gravity)
	{}

	void addBody(const Body& body)
	{
		_bodies.push_back(body);
	}

	void applyGravity()
	{
		for (Body& b : _bodies)
		{
			b.accelerate(_gravity);
		}
	}

	void solveBoundaryCollisions()
	{
		for (Body& b : _bodies)
		{
			Vec2 pos = b.position();
			float radius = b.radius();
			if (pos.x < radius)
			{
				b.move({ radius - pos.x, 0.0f });
			}
			else if (pos.x > _dimension.x - radius)
			{
				b.move({ _dimension.x - radius - pos.x, 0.0f });
			}

			if (pos.y < radius)
			{
				b.move({ 0.0f, radius - pos.y });
			}
			else if (pos.y > _dimension.y - radius)
			{
				b.move({ 0.0f, _dimension.y - radius - pos.y});
			}
		}
	}

	void solveInterbodiesCollisions()
	{
		for (Body& b1 : _bodies)
		{
			for (Body& b2 : _bodies)
			{
				if (&b1 == &b2)
					continue;

				Vec2 col_axe = b1.position() - b2.position();
				float col_radius = b1.radius() + b2.radius();

				if (col_axe.length2() < col_radius*col_radius)
				{
					float delta_col = 0.5f * (col_radius - col_axe.length());
					col_axe.normalize();

					b1.move(col_axe*delta_col);
					b2.move(col_axe*(-delta_col));
				}
			}
		}
	}

	void update(float dt)
	{
		applyGravity();

		for (int i(4); i--;)
		{
			solveInterbodiesCollisions();
		}

		solveBoundaryCollisions();

		for (Body& b : _bodies)
		{
			b.update(dt);
		}
	}

	const std::vector<Body>& bodies() const
	{
		return _bodies;
	}

private:
	Vec2 _dimension;
	Vec2 _gravity;
	std::vector<Body> _bodies;
};