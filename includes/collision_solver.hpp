#pragma once

#include "fast_array.hpp"
#include "vec2.hpp"
#include "physic_body.hpp"
#include "access_grid.hpp"
#include <SFML/System/Clock.hpp>
#include <iostream>
#include "segment.hpp"
#include "swarm.hpp"

constexpr uint32_t GRID_CELL_SIZE = 16U;

namespace up
{

	class CollisionSolver
	{
	public:
		CollisionSolver() = default;
		CollisionSolver(const Vec2& dimension, float body_radius, std::vector<Body>& bodies, const Vec2& gravity = Vec2(0.0f, 0.0f))
			: m_dimension(dimension)
			, m_gravity(gravity)
			, m_precision(1)
			, m_body_radius(body_radius)
			, m_grid(dimension, 2 * uint32_t(body_radius), bodies)
			, m_swarm(m_grid.getCells(), 6)
		{
			m_swarm.setJob([this](std::vector<GridCell<GRID_CELL_SIZE>>& data, uint32_t id, uint32_t step) {solveCollisionsSwarm(data, id, step); });
		}

		void update(fva::SwapArray<Body>& bodies, fva::SwapArray<SolidSegment>& segments, float dt)
		{
			applyGravity(bodies);

			collision_time = 0.0f;
			grid_time = 0.0f;

			sf::Clock clock_global, clock_local;

			clock_local.restart();
			solveBoundaryCollisions(bodies);

			m_grid.addBodies(bodies.getData());
			grid_time += clock_local.getElapsedTime().asMicroseconds() * 0.001f;

			for (uint32_t i(0); i<m_precision; ++i) {
				clock_local.restart();
				m_swarm.notifyReady();
				m_swarm.waitProcessed();
				collision_time += clock_local.getElapsedTime().asMicroseconds() * 0.001f;

				solveBodySegment(segments, bodies);
				solveSegmentSegment(segments);
				
				solveBoundaryCollisions(bodies);
			}

			total_update_time = clock_global.getElapsedTime().asMicroseconds() * 0.001f;
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
		float total_update_time;
		float collision_time;
		float grid_time;

	private:
		Vec2 m_gravity;
		Grid<GRID_CELL_SIZE> m_grid;
		Swarm<GridCell<GRID_CELL_SIZE>> m_swarm;

		const Vec2 m_dimension;
		const uint32_t m_precision;
		const float m_body_radius;

		void applyGravity(fva::SwapArray<Body>& bodies)
		{
			const Vec2 p(8000, 8000);
			const float force(900);

			for (Body& b : bodies) {
				b.accelerate(m_gravity);
				/*Vec2 dir(p - b.position());
				float length = dir.length();
				dir.normalize();
				b.accelerate(force * dir);*/
			}
		}

		void solveBoundaryCollisions(fva::SwapArray<Body>& bodies)
		{
			for (Body& b : bodies) {
				Vec2 pos = b.position();
				float radius = b.radius();
				if (pos.x < radius) {
					b.moveHard({ radius - pos.x, 0.0f });
					b.stop();
				} else if (pos.x > m_dimension.x - radius) {
					b.moveHard({ m_dimension.x - radius - pos.x, 0.0f });
					b.stop();
				}

				if (pos.y < radius) {
					b.moveHard({ 0.0f, radius - pos.y });
					b.stop();
				} else if (pos.y > m_dimension.y - radius) {
					b.moveHard({ 0.0f, m_dimension.y - radius - pos.y });
					b.stop();
				}
			}
		}

		void solveInterbodiesCollisions(float dt)
		{
			auto&  cr = m_grid.nonEmpty();
			for (auto* gc : cr) {
				const uint8_t size(gc->item_count);
				auto& bodies(gc->items);
				for (uint8_t i(0); i < size; ++i) {
					solveCellCollisions(i, size, bodies);
				}
			}
		}

		void solveCollisionsSwarm(std::vector<GridCell<GRID_CELL_SIZE>>& data, uint32_t id, uint32_t step)
		{
			const std::size_t size(data.size());

			uint32_t step_size(size / step);
			uint32_t start_index(id * step_size);
			uint32_t end_index(start_index + step_size);

			if (id%2) {
				for (uint32_t i(start_index); i<end_index; ++i) {
					GridCell<GRID_CELL_SIZE>& cell(data[i]);
					const uint8_t size(cell.item_count);
					auto& bodies(cell.items);
					for (uint8_t i(0); i < size; ++i) {
						solveCellCollisions(i, size, bodies);
					}
				}
			}
			else
			{
				for (uint32_t i(end_index-1); i > start_index; --i) {
					GridCell<GRID_CELL_SIZE>& cell(data[i]);
					const uint8_t size(cell.item_count);
					auto& bodies(cell.items);
					for (uint8_t i(0); i < size; ++i) {
						solveCellCollisions(i, size, bodies);
					}
				}
			}
		}

		void solveCellCollisions(uint32_t index, uint32_t size, std::array<up::Body*, GRID_CELL_SIZE>& bodies)
		{
			Body& b1(*bodies[index]);
			for (uint8_t i(index + 1); i < size; ++i) {
				Body& b2 = *bodies[i];
				solveBodiesCollision(b1, b2);
			}
		}

		void solveBodiesCollision(Body& b1, Body& b2)
		{
			const float prec_fact(1.0f / float(m_precision));

			const float col_radius(2*m_body_radius);
			Vec2 col_axe(b1.position() - b2.position());
			if (col_axe.length2() < col_radius*col_radius)
			{
				const Vec2 v1(b1.velocity());
				const Vec2 v2(b2.velocity());
				const Vec2 delta_v(v1 - v2);

				const float m1(b1.mass());
				const float m2(b2.mass());
				const float mass_factor_tot(1.0f / (m1 + m2));
				const float mass_factor_1(m1 * mass_factor_tot);
				const float mass_factor_2(m2 * mass_factor_tot);
				const float delta_col(0.5f * (col_radius - col_axe.length()));

				col_axe.normalize();
				b1.move(col_axe*(+delta_col * mass_factor_2));
				b2.move(col_axe*(-delta_col * mass_factor_1));

				const float cohesion(0.015f);
				b1.setVelocity(-cohesion*delta_v);
				b2.setVelocity( cohesion*delta_v);

				b1.addPressure(delta_col * prec_fact);
				b2.addPressure(delta_col * prec_fact);
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



