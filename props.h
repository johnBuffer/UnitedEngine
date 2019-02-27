#pragma once

#include "united_solver.h"
#include "FastVersatileArray.hpp"
#include <SFML/Graphics.hpp>
#include "DisplayManager.h"

class Rope
{
public:
	Rope() = default;

	Rope(const up::Vec2& p1, const up::Vec2& p2, float width, float steps) :
		_p1(p1),
		_p2(p2),
		_width(width),
		_steps(steps)
	{}

	void init(up::UnitedSolver& solver)
	{
		up::Vec2 dir = _p1 - _p2;
		float step_width = dir.length() / _steps;
		dir.normalize();

		up::BodyPtr b = solver.addBody(_p1, _width);
		_bodies.add(b);
		for (uint32_t i(1); i<_steps; ++i)
		{
			up::BodyPtr new_b = solver.addBody(_p1 + (i*step_width*dir), _width);
			up::ConstraintPtr c = solver.addConstraint(b, new_b);
			c->resitance(0.1f);
			_constraints.add(c);

			b = new_b;
			_bodies.add(b);
		}
	}

	void draw(sf::RenderTarget* target, DisplayManager& manager)
	{
		uint32_t constraints_count = _constraints.size();
		sf::VertexArray va(sf::Quads, constraints_count*4);

		uint32_t i(0);
		for (const up::ConstraintPtr c : _constraints)
		{
			if (c->broken())
				continue;

			up::Vec2 p1 = c->position1();
			up::Vec2 p2 = c->position2();

			up::Vec2 dir = p1 - p2;
			dir.normalize();

			up::Vec2 normal = _width*(dir.getNormal());

			up::Vec2 blp = manager.worldCoordToDisplayCoord(p1 - normal);
			up::Vec2 tlp = manager.worldCoordToDisplayCoord(p1 + normal);
			up::Vec2 trp = manager.worldCoordToDisplayCoord(p2 + normal);
			up::Vec2 brp = manager.worldCoordToDisplayCoord(p2 - normal);

			va[4 * i + 0].position = sf::Vector2f(blp.x, blp.y);
			va[4 * i + 1].position = sf::Vector2f(tlp.x, tlp.y);
			va[4 * i + 2].position = sf::Vector2f(trp.x, trp.y);
			va[4 * i + 3].position = sf::Vector2f(brp.x, brp.y);

			va[4 * i + 0].color = sf::Color::White;
			va[4 * i + 1].color = sf::Color::White;
			va[4 * i + 2].color = sf::Color::White;
			va[4 * i + 3].color = sf::Color::White;

			++i;
		}

		target->draw(va);
	}

private:
	fva::SwapArray<up::ConstraintPtr> _constraints;
	fva::SwapArray<up::BodyPtr> _bodies;

	float _width, _steps;
	up::Vec2 _p1, _p2;
};

class Wall
{
public:
	Wall() = default;

	Wall(const up::Vec2 position, uint32_t width, uint32_t height) :
		_position(position),
		_width(width),
		_height(height)
	{

	}

	void init(up::UnitedSolver& solver)
	{
		float radius = solver.defaultBodyRadius();

		float in_resistance = 50.0f;
		float out_resistance = 10.0f;
		float rope_resistance = 8.0f;

		up::BodyPtr b = solver.addBody(_position);
		_bodies.add(b);
		for (uint32_t j(1); j < _height; ++j)
		{
			up::BodyPtr new_b = solver.addBody(_position + up::Vec2(0, j * 2 * radius));

			solver.addAnchor(new_b, out_resistance);
			_bodies.add(new_b);

			auto c = solver.addConstraint(b, new_b);
			c->resitance(rope_resistance);
			b = new_b;
		}

		b = solver.addBody(_position + up::Vec2((_width-1) * 2 * radius, 0));
		_bodies.add(b);
		for (uint32_t j(1); j < _height; ++j)
		{
			up::BodyPtr new_b = solver.addBody(_position + up::Vec2((_width-1)*2*radius, j * 2 * radius));
			solver.addAnchor(new_b, out_resistance);
			_bodies.add(new_b);

			auto c = solver.addConstraint(b, new_b);
			c->resitance(rope_resistance);
			b = new_b;
		}

		for (uint32_t i(1); i < _width-1; ++i)
		{
			for (uint32_t j(0); j < _height; ++j)
			{
				up::BodyPtr new_b = solver.addBody(_position + up::Vec2(i * 2 * radius, j * 2 * radius));
				solver.addAnchor(new_b, in_resistance);
				_bodies.add(new_b);
			}
		}
	}

	void draw(sf::RenderTarget* target, DisplayManager& manager)
	{
		uint32_t bodies_count = _bodies.size();
		sf::VertexArray va(sf::Quads, bodies_count * 4);

		uint32_t i(0);
		for (const up::BodyPtr body : _bodies)
		{
			up::Vec2 position = body->position();
			float radius = body->radius();

			up::Vec2 blp = manager.worldCoordToDisplayCoord({ position.x - radius, position.y - radius });
			up::Vec2 tlp = manager.worldCoordToDisplayCoord({ position.x + radius, position.y - radius });
			up::Vec2 trp = manager.worldCoordToDisplayCoord({ position.x + radius, position.y + radius });
			up::Vec2 brp = manager.worldCoordToDisplayCoord({ position.x - radius, position.y + radius });

			va[4 * i + 0].position = sf::Vector2f(blp.x, blp.y);
			va[4 * i + 1].position = sf::Vector2f(tlp.x, tlp.y);
			va[4 * i + 2].position = sf::Vector2f(trp.x, trp.y);
			va[4 * i + 3].position = sf::Vector2f(brp.x, brp.y);

			va[4 * i + 0].color = sf::Color::White;
			va[4 * i + 1].color = sf::Color::White;
			va[4 * i + 2].color = sf::Color::White;
			va[4 * i + 3].color = sf::Color::White;

			++i;
		}

		//target->draw(va);
	}

private:
	fva::SwapArray<up::BodyPtr> _bodies;
	up::Vec2 _position;
	uint32_t _width, _height;
};
