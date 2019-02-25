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
	UnitedSolver(const Vec2& dimension, const Vec2& gravity = Vec2(0.0f,0.0f)) :
		_dimension(dimension),
		_gravity(gravity),
		_precision(1),
		_grid(dimension, 20)
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

	void solveInterbodiesCollisions(float dt)
	{	
		for (GridCell* gc : _grid.nonEmpty())
		{
			std::array<Body*, 10>& bodies = gc->items;

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
						col_axe.normalize();

						b1.move(col_axe*(final_delta*mass_factor_2));
						b2.move(col_axe*(-final_delta * mass_factor_1));

						b1.addPressure(delta_col);
						b2.addPressure(delta_col);
					}
				}
			}
		}
	}

	void update(float dt)
	{
		applyGravity();

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
		std::cout << "Collision time: " << update_time << "ms" << std::endl;

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

	bool test_pressure = true;

private:
	uint32_t _precision;
	Vec2 _dimension;
	Vec2 _gravity;

	std::vector<Body> _bodies;
	Grid _grid;
};