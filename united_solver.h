#pragma once

#include "FastVersatileArray.hpp"
#include "Body.h"
#include <vector>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include <algorithm>
#include "Grid.h"

class UnitedSolver
{
public:
	UnitedSolver() = default;
	UnitedSolver(const Vec2& dimension, uint32_t body_radius, const Vec2& gravity = Vec2(0.0f,0.0f)) :
		_dimension(dimension),
		_gravity(gravity),
		_precision(2),
		_body_radius(body_radius),
		_grid(dimension, 2*body_radius)
	{}

	void addBody(const Vec2& position)
	{
		_bodies.emplace_back(position, _body_radius, _bodies.size());
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
				b.stop();
			}
			else if (pos.x > _dimension.x - radius)
			{
				b.move({ _dimension.x - radius - pos.x, 0.0f });
				b.stop();
			}

			if (pos.y < radius)
			{
				b.move({ 0.0f, radius - pos.y });
				b.stop();
			}
			else if (pos.y > _dimension.y - radius)
			{
				b.move({ 0.0f, _dimension.y - radius - pos.y});
				b.stop();
			}
		}
	}

	void solveInterbodiesCollisions(float dt)
	{	
		for (auto* gc : _grid.nonEmpty())
		{
			auto& bodies = gc->items;

			float smooth_coef = 1.0f;
			float precision_factor = 1.0f / float(_precision);
			uint8_t size = gc->items_count;

			if (test_pressure)
				std::cout << "Size: " << uint32_t(size) << std::endl;

			for (uint8_t i(0); i<size; ++i)
			{
				Body& b1 = *bodies[i];
				for (uint8_t j(i+1); j < size; ++j)
				{
					Body& b2 = *bodies[j];
					Vec2 col_axe = b1.position() - b2.position();
					float col_radius = b1.radius() + b2.radius();

					if (col_axe.length2() < col_radius*col_radius)
					{
						float mass_factor_tot = 1.0f / (b1.mass() + b2.mass());
						float mass_factor_1 = b1.mass() * mass_factor_tot;
						float mass_factor_2 = b2.mass() * mass_factor_tot;

						float delta_col = precision_factor * 0.5f * (col_radius - col_axe.length());
						float final_delta = smooth_coef * delta_col;
						//float old_delta = final_delta * 0.25f;

						col_axe.normalize();

						b1.move(   col_axe*(final_delta * mass_factor_2));
						//b1.moveOld(col_axe*(old_delta   * mass_factor_2));

						b2.move(   col_axe*(-final_delta * mass_factor_1));
						//b2.moveOld(col_axe*(-old_delta   * mass_factor_1));

						b1.addPressure(final_delta);
						b2.addPressure(final_delta);
					}
				}
			}
		}
	}

	void update(float dt)
	{
		applyGravity();

		solveBoundaryCollisions();

		sf::Clock clock;
		for (int i(_precision); i--;)
		{
			_grid.clear();
			for (Body& b : _bodies)
			{
				_grid.addBody(b);
			}

			solveInterbodiesCollisions(dt);
		}
		const float update_time = clock.getElapsedTime().asMilliseconds();
		//std::cout << "Collision time: " << update_time << "ms" << std::endl;

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

	uint32_t bodyCount() const
	{
		return _bodies.size();
	}

	const Vec2& dimension() const
	{
		return _dimension;
	}

	bool test_pressure = false;

private:
	uint32_t _precision;
	Vec2 _dimension;
	Vec2 _gravity;

	std::vector<Body> _bodies;
	Grid _grid;

	const uint32_t _body_radius;
};