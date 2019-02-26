#pragma once

#include <array>
#include <vector>
#include "Body.h"

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

	void add(GridCell<7>& gc)
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

	std::vector<GridCell<7>*>::iterator begin()
	{
		return cells.begin();
	}

	std::vector<GridCell<7>*>::iterator end()
	{
		return cells.begin() + size;
	}

	uint32_t size;
	std::vector<GridCell<7>*> cells;
};

class Grid
{
public:
	Grid(const Vec2& dimension, uint32_t cell_size) :
		_cell_size(cell_size),
		_width(dimension.x / cell_size + 10),
		_height(dimension.y / cell_size + 10),
		_non_empty()
	{
		_non_empty.init(_width * _height);
		_cells.resize(_width * _height);
	}

	void addToCell(uint32_t grid_cell_x, uint32_t grid_cell_y, Body& b)
	{
		GridCell<7>& current_cell = _cells[grid_cell_x + _width *grid_cell_y];
		if (!current_cell.items_count)
			_non_empty.add(current_cell);

		current_cell.add(b);
	}

	void addBody(Body& b)
	{
		uint32_t body_x = b.position().x;
		uint32_t body_y = b.position().y;

		uint32_t grid_x = body_x / _cell_size + 5;
		uint32_t grid_y = body_y / _cell_size + 5;
		uint32_t mid_grid = _cell_size / 2;

		addToCell(grid_x, grid_y, b);

		uint32_t in_cell_x = body_x % _cell_size;
		uint32_t in_cell_y = body_y % _cell_size;

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
		return _non_empty;
	}

	void clear()
	{
		_non_empty.clear();
	}

private:
	uint32_t _cell_size;
	uint32_t _width;
	uint32_t _height;

	std::vector<GridCell<7>> _cells;
	CellRegister _non_empty;
};

}
