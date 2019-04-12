#pragma once

#include <array>
#include <vector>
#include "physic_body.hpp"

namespace up
{

template<int N>
struct GridCell
{
	GridCell() :
		items_count(0)
	{}

	void add(Body& b)
	{
		if (items_count < N)
		{
			items[items_count++] = &b;
		}
	}

	void clear()
	{
		for (uint8_t i(N); i--;)
		{
			items[i] = nullptr;
		}

		items_count = 0;
	}

	std::array<Body*, N> items;
	uint8_t items_count;
};

struct CellRegister
{
	CellRegister() :
		size(0)
	{}

	void init(uint32_t max_size)
	{
		cells.resize(max_size);
	}

	void add(GridCell<8>& gc)
	{
		cells[size++] = &gc;
	}

	void clear()
	{
		for (uint32_t i(0); i < size; ++i)
		{
			cells[i]->clear();
		}
		size = 0;
	}

	std::vector<GridCell<8>*>::iterator begin()
	{
		return cells.begin();
	}

	std::vector<GridCell<8>*>::iterator end()
	{
		return cells.begin() + size;
	}

	uint32_t size;
	std::vector<GridCell<8>*> cells;
};

class Grid
{
public:
	Grid(const Vec2& dimension, uint32_t cell_size) :
		m_cell_size(cell_size),
		m_width(uint32_t(dimension.x) / cell_size + 10),
		m_height(uint32_t(dimension.y) / cell_size + 10),
		m_non_empty()
	{
		m_non_empty.init(m_width * m_height);
		m_cells.resize(m_width * m_height);
	}

	void addToCell(uint32_t grid_cell_x, uint32_t grid_cell_y, Body& b)
	{
		GridCell<8>& current_cell = m_cells[grid_cell_x + m_width *grid_cell_y];
		if (!current_cell.items_count)
			m_non_empty.add(current_cell);

		current_cell.add(b);
	}

	void addBody(Body& b)
	{
		uint32_t body_x = uint32_t(b.position().x + 1.0f);
		uint32_t body_y = uint32_t(b.position().y + 1.0f);

		uint32_t grid_x = body_x / m_cell_size + 5;
		uint32_t grid_y = body_y / m_cell_size + 5;
		uint32_t mid_grid = m_cell_size / 2;

		addToCell(grid_x, grid_y, b);

		uint32_t in_cell_x = body_x % m_cell_size;
		uint32_t in_cell_y = body_y % m_cell_size;

		if (in_cell_x > mid_grid)
		{
			addToCell(grid_x + 1, grid_y, b);
			if (in_cell_y > mid_grid)
			{
				addToCell(grid_x    , grid_y + 1, b);
				addToCell(grid_x + 1, grid_y + 1, b);
			}
			else
			{
				addToCell(grid_x    , grid_y - 1, b);
				addToCell(grid_x + 1, grid_y - 1, b);
			}
		}
		else
		{
			addToCell(grid_x - 1, grid_y, b);
			if (in_cell_y > mid_grid)
			{
				addToCell(grid_x, grid_y + 1, b);
				addToCell(grid_x - 1, grid_y + 1, b);
			}
			else
			{
				addToCell(grid_x, grid_y - 1, b);
				addToCell(grid_x - 1, grid_y - 1, b);
			}
		}
	}

	CellRegister& nonEmpty()
	{
		return m_non_empty;
	}

	void clear()
	{
		m_non_empty.clear();
	}

private:
	uint32_t m_cell_size;
	uint32_t m_width;
	uint32_t m_height;

	std::vector<GridCell<8>> m_cells;
	CellRegister m_non_empty;
};

}
