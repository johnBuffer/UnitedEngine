#pragma once

#include <array>
#include <vector>
#include "Body.h"

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

class Grid
{
public:
	Grid(const Vec2& dimension, uint32_t cell_size) :
		_cell_size(cell_size),
		_width(dimension.x / cell_size + 10),
		_height(dimension.y / cell_size + 10)
	{
		_cells.resize(_width);
		for (std::vector<GridCell<10>>& column : _cells)
		{
			column.resize(_height);
		}
	}

	void addToCell(uint32_t grid_cell_x, uint32_t grid_cell_y, Body& b)
	{
		/*if (grid_cell_x < 0 || grid_cell_x > _width || grid_cell_y < 0 || grid_cell_y > _height)
			return;*/

		GridCell<10>& current_cell = _cells[grid_cell_x][grid_cell_y];
		if (!current_cell.items_count)
			_non_empty.push_back(&current_cell);

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

	std::vector<GridCell<10>*>& nonEmpty()
	{
		return _non_empty;
	}

	void clear()
	{
		_non_empty.clear();

		for (std::vector<GridCell<10>>& column : _cells)
		{
			for (GridCell<10>& cell : column)
			{
				cell.clear();
			}
		}
	}

private:
	uint32_t _cell_size;
	uint32_t _width;
	uint32_t _height;

	std::vector< std::vector<GridCell<10>> > _cells;
	std::vector< GridCell<10>* > _non_empty;
};
