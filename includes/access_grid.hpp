#pragma once

#include <array>
#include <vector>
#include "physic_body.hpp"
#include "segment.hpp"
#include <thread>
#include "condition.hpp"


namespace up
{

// GridCell class
template<int N>
struct GridCell
{
	GridCell()
		: item_count(0)
	{}

	GridCell(const GridCell<N>& cell)
		: item_count(cell.item_count)
		, items(cell.items)
	{}

	GridCell<N>& operator=(const GridCell<N>& cell)
	{
		item_count = cell.item_count;
		items = cell.items;

		return *this;
	}

	void add(Body& b)
	{
		if (item_count < N) {
			items[item_count++] = &b;
		}
	}

	void add(SolidSegment& s)
	{
		segments.push_back(&s);
	}

	void clear()
	{
		for (uint8_t i(N); i--;) {
			items[i] = nullptr;
		}
		item_count = 0;
	}

	std::array<Body*, N> items;
	std::vector<SolidSegment*> segments;
	uint8_t item_count;
};


// CellRegister class
template<uint8_t N>
struct CellRegister
{
	CellRegister() :
		size(0)
	{}

	void init(uint32_t max_size)
	{
		cells.resize(max_size);
	}

	void add(GridCell<N>& gc)
	{
		cells[size++] = &gc;
	}

	void clear()
	{
		for (uint32_t i(0); i < size; ++i) {
			cells[i]->clear();
		}
		size = 0;
	}

	typename std::vector<GridCell<N>*>::iterator begin()
	{
		return cells.begin();
	}

	typename std::vector<GridCell<N>*>::iterator end()
	{
		return cells.begin() + size;
	}

	uint32_t size;
	std::vector<GridCell<N>*> cells;
};


template<uint8_t N>
struct GridWorker;


// Grid class
template<uint8_t N>
class Grid
{
public:
	Grid(const Vec2& dimension, uint32_t cell_size, std::vector<up::Body>& bodies)
		: m_cell_size(cell_size)
		, m_width(uint32_t(dimension.x) / cell_size + 10)
		, m_height(uint32_t(dimension.y) / cell_size + 10)
		, m_non_empty()
		, m_condition1()
		, m_worker1(*this, bodies, 0, 1, m_condition1)
	{
		m_non_empty.init(m_width * m_height);
		m_cells.resize(m_width * m_height);
	}

	void addToCell(uint32_t grid_cell_x, uint32_t grid_cell_y, Body& b)
	{
		GridCell<N>& current_cell = m_cells[grid_cell_x + m_width*grid_cell_y];
		if (!current_cell.item_count)
			m_non_empty.add(current_cell);

		current_cell.add(b);
	}

	GridCell<N>* getColliders(const Body& b)
	{
		uint32_t body_x = uint32_t(b.position().x + 1.0f);
		uint32_t body_y = uint32_t(b.position().y + 1.0f);

		uint32_t grid_x = body_x / m_cell_size + 5;
		uint32_t grid_y = body_y / m_cell_size + 5;

		if (grid_x > 0 && grid_x < m_width &&
			grid_y > 0 && grid_y < m_height) {
			return &m_cells[grid_x + m_width * grid_y];
		}

		return nullptr;
	}

	void addBodies(std::vector<up::Body>& bodies)
	{
		clear();
		m_condition1.notifyReady();
		m_condition1.waitProcessed();
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

		if (in_cell_x > mid_grid) {
			addToCell(grid_x + 1, grid_y, b);
			if (in_cell_y > mid_grid) {
				addToCell(grid_x    , grid_y + 1, b);
				addToCell(grid_x + 1, grid_y + 1, b);
			} else {
				addToCell(grid_x    , grid_y - 1, b);
				addToCell(grid_x + 1, grid_y - 1, b);
			}
		} else {
			addToCell(grid_x - 1, grid_y, b);
			if (in_cell_y > mid_grid) {
				addToCell(grid_x, grid_y + 1, b);
				addToCell(grid_x - 1, grid_y + 1, b);
			} else {
				addToCell(grid_x, grid_y - 1, b);
				addToCell(grid_x - 1, grid_y - 1, b);
			}
		}
	}

	CellRegister<N>& nonEmpty()
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

	std::vector<GridCell<N>> m_cells;
	CellRegister<N> m_non_empty;

	Condition m_condition1;
	//Condition m_condition2;
	GridWorker<N> m_worker1;
	//GridWorker<N> m_worker2;
};

template<uint8_t N>
struct GridWorker
{
	GridWorker(Grid<N>& grid_, std::vector<up::Body>& data_, uint32_t id_, uint32_t step_, Condition& condition)
		: grid(grid_)
		, data(data_)
		, id(id_)
		, step(step_)
		, sync_condition(condition)
	{
		job_thread = std::thread(&GridWorker<10>::job, this);
	}

	void job()
	{
		while (true) {
			sync_condition.waitReady();
			const uint32_t size(data.size());
			for (uint32_t i(id); i < size; i += step) {
				grid.addBody(data[i]);
			}
			sync_condition.notifyProcessed();
		}
	}

	Grid<N>& grid;
	const uint32_t id;
	const uint32_t step;
	std::vector<up::Body>& data;

	std::mutex mutex;
	bool done;

	Condition& sync_condition;
	std::thread job_thread;
};

}
