#pragma once

#include "fast_array.hpp"
#include "vec2.hpp"
#include "physic_body.hpp"
#include "access_grid.hpp"
#include <SFML/System/Clock.hpp>
#include <iostream>
#include "segment.hpp"

namespace up
{

	class CollisionSolver
	{
	public:
		CollisionSolver() = default;
		CollisionSolver(const Vec2& dimension, float body_radius, const Vec2& gravity = Vec2(0.0f, 0.0f)) :
			m_dimension(dimension),
			m_gravity(gravity),
			m_precision(4),
			m_body_radius(body_radius),
			m_grid(dimension, 2 * uint32_t(body_radius))
		{}

		void update(fva::SwapArray<Body>& bodies, fva::SwapArray<SolidSegment>& segments, float dt)
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

				solveBodySegment(segments, bodies);
				/*solveSegmentSegment(segments);*/
				solveBoundaryCollisions(bodies);
			}

			m_up_time = clock.getElapsedTime().asMicroseconds() * 0.001f;
		}

		const Vec2& dimension() const
		{
			return m_dimension;
		}

		float defaultBodyRadius() const
		{
			return m_body_radius;
		}

		bool test_pressure = false;
		float m_up_time;

	private:
		Vec2 m_gravity;
		Grid<10U> m_grid;

		const Vec2 m_dimension;
		const uint32_t m_precision;
		const float m_body_radius;

		void applyGravity(fva::SwapArray<Body>& bodies)
		{
			for (Body& b : bodies) {
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
					b.moveHard({ radius - pos.x, 0.0f });
					b.stop();
				}
				else if (pos.x > m_dimension.x - radius)
				{
					b.moveHard({ m_dimension.x - radius - pos.x, 0.0f });
					b.stop();
				}

				if (pos.y < radius)
				{
					b.moveHard({ 0.0f, radius - pos.y });
					b.stop();
				}
				else if (pos.y > m_dimension.y - radius)
				{
					b.moveHard({ 0.0f, m_dimension.y - radius - pos.y });
					b.stop();
				}
			}
		}

		void solveInterbodiesCollisions(float dt)
		{
			auto&  cr = m_grid.nonEmpty();
			for (auto* gc : cr) {
				const uint8_t size(gc->items_count);
				auto& bodies(gc->items);
				for (uint8_t i(0); i < size; ++i) {
					solveCellCollisions(i, size, bodies);
				}
			}
		}

		void solveCellCollisions(uint32_t index, uint32_t size, std::array<up::Body*, 10U>& bodies)
		{
			Body& b1(*bodies[index]);
			for (uint8_t i(index + 1); i < size; ++i) {
				Body& b2 = *bodies[i];
				solveBodiesCollision(b1, b2);
			}
		}

		void solveBodiesCollision(Body& b1, Body& b2)
		{
			const float col_radius(2*m_body_radius);
			Vec2 col_axe(b1.position() - b2.position());
			if (col_axe.length2() < col_radius*col_radius)
			{
				const float m1(b1.mass());
				const float m2(b2.mass());
				const float mass_factor_tot(1.0f / (m1 + m2));
				const float mass_factor_1(m1 * mass_factor_tot);
				const float mass_factor_2(m2 * mass_factor_tot);
				const float delta_col(0.5f * (col_radius - col_axe.length()));

				col_axe.normalize();
				b1.move(col_axe*(+delta_col * mass_factor_2));
				b2.move(col_axe*(-delta_col * mass_factor_1));
				
				b1.addPressure(delta_col);
				b2.addPressure(delta_col);
			}
		}

		void solveBodySegment(fva::SwapArray<SolidSegment>& segments, fva::SwapArray<Body>& bodies)
		{
			for (SolidSegment& s : segments) {
				for (Body& b : bodies) {
					s.collideWith(b);
				}
			}
		}

		void solveSegmentSegment(fva::SwapArray<SolidSegment>& segments)
		{
			for (SolidSegment& s1 : segments) {
				for (SolidSegment& s2 : segments) {
					if (&s1 != &s2) {
						s1.collideWith(s2);
					}
				}
			}
		}
	};
}



