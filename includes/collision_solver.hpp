#pragma once

#include "fast_array.hpp"
#include "vec2.hpp"
#include "physic_body.hpp"
#include "access_grid.hpp"
#include <SFML/System/Clock.hpp>
#include <iostream>

namespace up
{

	class CollisionSolver
	{
	public:
		CollisionSolver() = default;
		CollisionSolver(const Vec2& dimension, float body_radius, const Vec2& gravity = Vec2(0.0f, 0.0f)) :
			m_dimension(dimension),
			m_gravity(gravity),
			m_precision(2),
			m_body_radius(body_radius),
			m_grid(dimension, 2 * uint32_t(body_radius))
		{}

		void update(fva::SwapArray<Body>& bodies, float dt)
		{
			applyGravity(bodies);
			sf::Clock clock;
			for (uint32_t i(0); i<m_precision; ++i) {
				solveBoundaryCollisions(bodies);
				m_grid.clear();
				for (Body& b : bodies) {
					m_grid.addBody(b);
				}
				solveInterbodiesCollisions(dt);
			}

			m_up_time = clock.getElapsedTime().asMicroseconds() * 0.001f;

			solveBoundaryCollisions(bodies);
			for (Body& b : bodies) {
				b.update(dt);
			}
		}

		const Vec2& dimension() const
		{
			return m_dimension;
		}

		/*void applyExplosion(const Vec2& position, float force)
		{
			for (Body& b : m_bodies)
			{
				Vec2 dir = b.position() - position;
				float length = dir.length();
				dir.normalize();

				float force_factor = force / (length + 1.0f);

				b.accelerate(force_factor * dir);
			}
		}*/

		float defaultBodyRadius() const
		{
			return m_body_radius;
		}

		bool test_pressure = false;
		float m_up_time;

	private:
		Vec2 m_gravity;
		Grid<8U> m_grid;

		const Vec2 m_dimension;
		const uint32_t m_precision;
		const float m_body_radius;

		void applyGravity(fva::SwapArray<Body>& bodies)
		{
			const Vec2 attract(12500, 25000);
			for (Body& b : bodies)
			{
				b.accelerate(m_gravity);
			}
		}

		void solveBoundaryCollisions(fva::SwapArray<Body>& bodies)
		{
			for (Body& b : bodies)
			{
				Vec2 pos = b.position();
				float radius = b.radius();
				if (pos.x < radius)
				{
					b.move({ radius - pos.x, 0.0f });
					b.stop();
				}
				else if (pos.x > m_dimension.x - radius)
				{
					b.move({ m_dimension.x - radius - pos.x, 0.0f });
					b.stop();
				}

				if (pos.y < radius)
				{
					b.move({ 0.0f, radius - pos.y });
					b.stop();
				}
				else if (pos.y > m_dimension.y - radius)
				{
					b.move({ 0.0f, m_dimension.y - radius - pos.y });
					b.stop();
				}
			}
		}

		void solveInterbodiesCollisions(float dt)
		{
			const float col_radius(2.0f * m_body_radius);

			auto&  cr = m_grid.nonEmpty();
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
							const float m1(b1.mass());
							const float m2(b2.mass());
							const float mass_factor_tot(1.0f / (m1 + m2));
							const float mass_factor_1(m1 * mass_factor_tot);
							const float mass_factor_2(m2 * mass_factor_tot);
							const float delta_col(0.5f * (col_radius - col_axe.length()));

							col_axe.normalize();
							b1.move(col_axe*(delta_col * mass_factor_2));
							b2.move(col_axe*(-delta_col * mass_factor_1));
							b1.addPressure(delta_col);
							b2.addPressure(delta_col);
						}
					}
				}
			}
		}
	};
}