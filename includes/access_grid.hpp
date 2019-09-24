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
	template<uint32_t N>
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
			if (b.debug) {
				debug = true;
				for (uint32_t i(0); i < item_count; ++i) {
					items[i]->debug_collision = true;
				}
			}

			if (item_count < N) {
				items[item_count++] = &b;
				if (debug)
					b.debug_collision = true;
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

		void clear_bodies()
		{
			for (uint32_t i(N); i--;) {
				items[i] = nullptr;
			}

			item_count = 0;
			debug = false;
		}

		void clear_segments()
		{
			segments.clear();
			segment_count = 0;
		}

		std::array<Body*, N> items;
		std::vector<SolidSegment*> segments;
		uint32_t item_count;
		uint8_t segment_count;
		bool debug;
	};


	// Grid class
	template<uint32_t N>
	class Grid
	{
	public:
		Grid(const Vec2& dimension, uint32_t cell_size, std::vector<up::Body>& bodies)
			: m_cell_size(cell_size)
			, m_width(uint32_t(dimension.x) / cell_size + 10)
			, m_height(uint32_t(dimension.y) / cell_size + 10)
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
			getCellAt(grid_cell_x, grid_cell_y).add(s);
		}

		GridCell<N>* getColliders(const Body& b)
		{
			uint32_t body_x = uint32_t(b.position().x);
			uint32_t body_y = uint32_t(b.position().y);

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
			clear_bodies();
			m_swarm.notifyReady();
			m_swarm.waitProcessed();
		}

		void addSegments(std::vector<up::SolidSegment>& segments)
		{
			clear_segments();
			for (up::SolidSegment& seg : segments) {
				add(seg);
			}
		}

		void addBodiesSwarm(std::vector<Body>& data, uint32_t id, uint32_t step)
		{
			const std::size_t size(data.size());
			for (std::size_t i(id); i < size; i += step) {
				add(data[i]);
			}
		}

		void vec2ToGridCoord(const Vec2& v, int32_t& grid_x, int32_t& grid_y)
		{
			grid_x = int32_t(v.x) / m_cell_size + 5;
			grid_y = int32_t(v.y) / m_cell_size + 5;
		}

		void add(Body& b)
		{
			const float radius(b.radius);
			const Vec2& position(b.position());
			
			int32_t cell_x, cell_y;
			vec2ToGridCoord(position, cell_x, cell_y);

			const float cell_left ((cell_x-5) * m_cell_size);
			const float cell_right((cell_x-4) * m_cell_size);
			const float cell_bot  ((cell_y-5) * m_cell_size);
			const float cell_top  ((cell_y-4) * m_cell_size);

			addToCell(cell_x, cell_y, b);
			/*addToCell(cell_x+1, cell_y, b);
			addToCell(cell_x-1, cell_y, b);
			addToCell(cell_x + 1, cell_y + 1, b);
			addToCell(cell_x + 1, cell_y - 1, b);
			addToCell(cell_x - 1, cell_y + 1, b);
			addToCell(cell_x - 1, cell_y - 1, b);
			addToCell(cell_x, cell_y - 1, b);
			addToCell(cell_x, cell_y + 1, b);*/

			const float delta_bot(position.y - cell_bot);
			const float delta_top(cell_top - position.y);
			const float delta_left(position.x - cell_left);
			const float delta_right(cell_right - position.x);

			if (delta_left < radius) {
				addToCell(cell_x - 1, cell_y, b);
				if (delta_top < radius) {
					addToCell(cell_x - 1, cell_y + 1, b);
					addToCell(cell_x    , cell_y + 1, b);
				} else if (delta_bot < radius) {
					addToCell(cell_x - 1, cell_y - 1, b);
					addToCell(cell_x    , cell_y - 1, b);
				}
			} else if (delta_right < radius) {
				addToCell(cell_x + 1, cell_y, b);
				if (delta_top <= radius) {
					addToCell(cell_x + 1, cell_y + 1, b);
					addToCell(cell_x    , cell_y + 1, b);
				} else if (delta_bot < radius) {
					addToCell(cell_x + 1, cell_y - 1, b);
					addToCell(cell_x    , cell_y - 1, b);
				}
			} else if (delta_top < radius) {
				addToCell(cell_x, cell_y + 1, b);
			} else if (delta_bot < radius) {
				addToCell(cell_x, cell_y - 1, b);
			}
		}

		void add(SolidSegment& seg)
		{
			const float x1 = seg.getBody1Position().x;
			const float x2 = seg.getBody2Position().x;
			const float y1 = seg.getBody1Position().y;
			const float y2 = seg.getBody2Position().y;

			const float dx(x2 - x1);
			const float dy(y2 - y1);

			const float length = seg.getVec().length();

			const float seg_dx = dx / length;
			const float seg_dy = dy / length;

			/// cell_x and cell_y are the starting voxel's coordinates
			int32_t cell_x;// = x1 / m_cell_size;
			int32_t cell_y;// = y1 / m_cell_size;

			vec2ToGridCoord(seg.getBody1Position(), cell_x, cell_y);

			int32_t end_x;// = x2 / m_cell_size;
			int32_t end_y;// = y2 / m_cell_size;

			vec2ToGridCoord(seg.getBody2Position(), end_x, end_y);

			/// step_x and step_y describe if cell_x and cell_y
			/// are incremented or decremented during iterations
			int step_x = seg_dx < 0 ? -1 : 1;
			int step_y = seg_dy < 0 ? -1 : 1;

			/// Compute the value of t for first intersection in x and y
			float t_max_x = 0;
			if (step_x > 0)
				t_max_x = (cell_x - 4) * m_cell_size - x1;
			else
				t_max_x = (cell_x - 5) * m_cell_size - x1;
			t_max_x /= seg_dx;

			float t_max_y = 0;
			if (step_y > 0)
				t_max_y = (cell_y - 4) * m_cell_size - y1;
			else
				t_max_y = (cell_y - 5) * m_cell_size - y1;
			t_max_y /= seg_dy;

			/// Compute how much (in units of t) we can move along the ray
			/// before reaching the cell's width and height
			float t_dx = std::abs(float(m_cell_size) / seg_dx);
			float t_dy = std::abs(float(m_cell_size) / seg_dy);

			while (1) {
				addToCell(cell_x, cell_y, seg);

				if (t_max_x < t_max_y) {
					t_max_x += t_dx;
					cell_x += step_x;
				}
				else {
					t_max_y += t_dy;
					cell_y += step_y;
				}

				if ((step_x > 0 && cell_x > end_x) || (step_x < 0 && cell_x < end_x)
					|| (step_y > 0 && cell_y > end_y) || (step_y < 0 && cell_y < end_y)) {
					break;
				}
			}

			addToCell(cell_x, cell_y, seg);
		}

		void clear_bodies()
		{
			for (GridCell<N>& cell : m_cells) {
				cell.clear_bodies();
			}
		}

		void clear_segments()
		{
			for (GridCell<N>& cell : m_cells) {
				cell.clear_segments();
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

		const GridCell<N>& getCellAt(uint32_t grid_cell_x, uint32_t grid_cell_y) const
		{
			return m_cells[grid_cell_y + m_height * grid_cell_x];
		}

		GridCell<N>& getCellAt(uint32_t grid_cell_x, uint32_t grid_cell_y)
		{
			return m_cells[grid_cell_y + m_height * grid_cell_x];
		}

		up::Vec2 getSize() const
		{
			return up::Vec2(m_width, m_height);
		}

		uint32_t getCellSize() const
		{
			return m_cell_size;
		}

	private:
		uint32_t m_cell_size;
		uint32_t m_width;
		uint32_t m_height;

		std::vector<GridCell<N>> m_cells;
		Swarm<up::Body> m_swarm;
	};


}
