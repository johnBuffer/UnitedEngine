#pragma once

#include <array>
#include <vector>
#include "physic_body.hpp"
#include "segment.hpp"
#include "swarm.hpp"
#include <iostream>


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
			/*if (b.debug) {
				debug = true;
				for (int i(0); i < item_count; ++i) {
					items[i]->debug_colision = true;
				}
			}*/

			if (item_count < N) {
				items[item_count++] = &b;
				/*if (debug)
					b.debug_colision = true;*/
			}
		}

		/*void print_debug() const
		{
			if (debug)
				std::cout << "Item count: " << uint32_t(item_count) << std::endl;
		}*/

		void add(SolidSegment& s)
		{
			segments.push_back(&s);
			++segment_count;
		}

		void clear()
		{
			for (uint8_t i(N); i--;) {
				items[i] = nullptr;
			}

			segments.clear();

			item_count = 0;
			segment_count = 0;

			//debug = false;
		}

		std::array<Body*, N> items;
		std::vector<SolidSegment*> segments;
		uint8_t item_count;
		uint8_t segment_count;
		//bool debug;
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
			, m_swarm(bodies, 1)
		{
			//m_non_empty.init(m_width * m_height);
			m_cells.resize(m_width * m_height);

			m_swarm.setJob([this](std::vector<Body>& data, uint32_t id, uint32_t step) {addBodiesSwarm(data, id, step); });
		}

		void addToCell(uint32_t grid_cell_x, uint32_t grid_cell_y, Body& b)
		{
			GridCell<N>& current_cell = m_cells[grid_cell_y + m_height * grid_cell_x];
			/*if (!current_cell.item_count)
				m_non_empty.add(current_cell);*/

			current_cell.add(b);
		}

		void addToCell(uint32_t grid_cell_x, uint32_t grid_cell_y, SolidSegment& s)
		{
			GridCell<N>& current_cell = m_cells[grid_cell_y + m_height * grid_cell_x];
			current_cell.add(s);
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
			m_swarm.notifyReady();
			m_swarm.waitProcessed();
		}

		void addBodiesSwarm(std::vector<Body>& data, uint32_t id, uint32_t step)
		{
			const std::size_t size(data.size());
			for (std::size_t i(id); i < size; i += step) {
				add(data[i]);
			}
		}

		void vec2ToGridCoord(const Vec2& v, uint32_t& grid_x, uint32_t& grid_y)
		{
			int32_t body_x = uint32_t(v.x);
			int32_t body_y = uint32_t(v.y);

			grid_x = body_x / m_cell_size + 5;
			grid_y = body_y / m_cell_size + 5;
		}

		void add(Body& b)
		{
			const float radius(b.radius());
			const Vec2& position(b.position());
			
			uint32_t grid_x, grid_y;
			vec2ToGridCoord(position, grid_x, grid_y);

			const float grid_left(float(grid_x)*m_cell_size);
			const float grid_right(float(grid_x + 1)*m_cell_size);
			const float grid_top(float(grid_y + 1)*m_cell_size);
			const float grid_bot(float(grid_y)*m_cell_size);

			addToCell(grid_x, grid_y, b);

			const float delta_top(position.y - grid_top);
			const float delta_bot(grid_bot - position.y);

			if (position.x - grid_left <= radius) {
				addToCell(grid_x - 1, grid_y, b);
				if (delta_top <= radius) {
					addToCell(grid_x - 1, grid_y - 1, b);
					addToCell(grid_x, grid_y - 1, b);
				} else if (delta_bot) {
					addToCell(grid_x - 1, grid_y + 1, b);
					addToCell(grid_x, grid_y + 1, b);
				}
			} else if (grid_right - position.x <= radius) {
				addToCell(grid_x + 1, grid_y, b);
				if (delta_top <= radius) {
					addToCell(grid_x + 1, grid_y - 1, b);
					addToCell(grid_x, grid_y - 1, b);
				} else if (delta_bot < radius) {
					addToCell(grid_x + 1, grid_y + 1, b);
					addToCell(grid_x, grid_y + 1, b);
				}
			} else if (position.y - grid_top <= radius) {
				addToCell(grid_x, grid_y - 1, b);
			} else if (grid_bot - position.y <= radius) {
				addToCell(grid_x, grid_y + 1, b);
			}
		}

		void add(SolidSegment& s)
		{
			const Vec2& start_position(s.getBody1Position());
			const Vec2& end_position(s.getBody2Position());

			const float delta_x = end_position.x - start_position.x;
			const float delta_y = end_position.y - start_position.y;
			const float delta_e = std::abs(delta_y / delta_x);
			const float error = 0.0f;
			const int32_t delta_y_sign = delta_y < 0.0f ? -1 : 1;

			uint32_t x0, y0, x1, y1;
			vec2ToGridCoord(start_position, x0, y0);
			vec2ToGridCoord(end_position, x1, y1);
			uint32_t y(y0);
			for (int32_t x(x0); x <= x1; ++x) {
				addToCell(x, y, s);
				error += delta_e;
				if (error >= 0.5f) {
					y += delta_y_sign;
					error -= 1.0;
				}
			}
		}

		CellRegister<N>& nonEmpty()
		{
			return m_non_empty;
		}

		void clear()
		{
			for (GridCell<N>& cell : m_cells) {
				cell.clear();
			}
		}

		/*void print_debug() const
		{
			for (const GridCell<N>& cell : m_cells) {
				cell.print_debug();
			}
		}*/

		std::vector<GridCell<N>>& getCells()
		{
			return m_cells;
		}

	private:
		uint32_t m_cell_size;
		uint32_t m_width;
		uint32_t m_height;

		std::vector<GridCell<N>> m_cells;
		CellRegister<N> m_non_empty;
		Swarm<up::Body> m_swarm;
	};


}
