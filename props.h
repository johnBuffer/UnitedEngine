#pragma once

#include "united_solver.h"
#include "FastVersatileArray.hpp"
#include <SFML/Graphics.hpp>

class Rope
{
public:
	Rope() = default;

	void draw(sf::RenderTarget* target)
	{
		uint32_t constraints_count = _constraints.size();
		sf::VertexArray va(sf::Quads, constraints_count*4);

		uint32_t i(0);
		for (const up::ConstraintPtr c : _constraints)
		{
			va[4 * i + 0].position = sf::Vector2f();
		}
	}

private:
	fva::SwapArray<up::ConstraintPtr> _constraints;
};

class Wall
{
public:
	Wall() = default;

private:

};
