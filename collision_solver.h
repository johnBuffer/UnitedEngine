#pragma once

#include "FastVersatileArray.hpp"
#include "vec2.h"
#include "Body.h"
#include "Grid.h"
#include <SFML/System/Clock.hpp>
#include <iostream>

namespace up
{

	class CollisionSolver
	{
	public:
		CollisionSolver() = default;
		CollisionSolver(const Vec2& dimension, uint32_t body_radius, const Vec2& gravity = Vec2(0.0f, 0.0f)) :
			_dimension(dimension),
			_gravity(gravity),
			_precision(1),
			_body_radius(body_radius),
			_grid(dimension, 2 * body_radius)
		{}

		fva::Handle<Body> addBody(const Vec2& position, float radius)
		{
			if (radius > 0.0f && radius <= _body_radius)
				return _bodies.add(position, radius);
			
			return _bodies.add(position, _body_radius);
		}

		void applyGravity()
		{
			const Vec2 attract(12500, 25000);
			for (Body& b : _bodies)
			{
				Vec2 dir = attract - b.position();
				dir.normalize();

				b.accelerate(500.0f*dir);
				//b.accelerate(_gravity);
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
					b.move({ 0.0f, _dimension.y - radius - pos.y });
					b.stop();
				}
			}
		}

		void solveInterbodiesCollisions(float dt)
		{
			const float col_radius = 2 * _body_radius;

			CellRegister&  cr = _grid.nonEmpty();
			for (auto* gc : cr)
			{
				const uint8_t size = gc->items_count;
				auto& bodies = gc->items;

				for (uint8_t i(0); i < size; ++i)
				{
					Body& b1 = *bodies[i];
					for (uint8_t j(i + 1); j < size; ++j)
					{
						Body& b2 = *bodies[j];
						Vec2 col_axe = b1.position() - b2.position();

						if (col_axe.length2() < col_radius*col_radius)
						{
							const float m1 = b1.mass();
							const float m2 = b2.mass();
							const float mass_factor_tot = 1.0f / (m1 + m2);
							const float mass_factor_1 = m1 * mass_factor_tot;
							const float mass_factor_2 = m2 * mass_factor_tot;

							const float delta_col = 0.5f * (col_radius - col_axe.length());

							col_axe.normalize();

							b1.move(col_axe*( delta_col * mass_factor_2));
							b2.move(col_axe*(-delta_col * mass_factor_1));

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
			for (int i(0); i<_precision; ++i)
			{
				solveBoundaryCollisions();
				_grid.clear();
				for (Body& b : _bodies)
				{
					_grid.addBody(b);
				}

				solveInterbodiesCollisions(dt);
			}
			_up_time = clock.getElapsedTime().asMicroseconds() * 0.001f;

			solveBoundaryCollisions();

			for (Body& b : _bodies)
			{
				b.update(dt);
			}
		}

		const fva::SwapArray<Body>& bodies() const
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

		void applyExplosion(const Vec2& position, float force)
		{
			for (Body& b : _bodies)
			{
				Vec2 dir = b.position() - position;
				float length = dir.length();
				dir.normalize();

				float force_factor = force / (length + 1.0f);

				b.accelerate(force_factor * dir);
			}
		}

		float defaultBodyRadius() const
		{
			return _body_radius;
		}

		bool test_pressure = false;
		float _up_time;


	private:
		Vec2 _gravity;

		fva::SwapArray<Body> _bodies;
		fva::SwapArray<FastCollider> _colliders;
		Grid _grid;

		const Vec2 _dimension;
		const uint32_t _precision;
		const uint32_t _body_radius;
	};

}