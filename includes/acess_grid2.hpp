#pragma once
#include <vector>
#include <array>
#include "physic_body.hpp"

namespace mtg
{
	struct GridCell
	{
		GridCell()
			: data_size(0U)
			, sides_size(0U)
		{}

		void addBody(up::Body& b)
		{
			data.push_back(b);
			++data_size;
		}

		void addBodyFromOhterCell(GridCell& cell, uint32_t index)
		{
			addBody(cell.data[index]);
			cell.removeBody(index);
		}

		void removeBody(uint32_t index)
		{
			if (data_size)
			{
				--data_size;
				data[index] = data[data_size];
				data.pop_back();
			}
		}

		void checkBodies(std::vector<up::Body>& out, std::vector<uint32_t>& indexes)
		{
			for (uint32_t i(0); i<data_size; ++i) {
				const up::Body& b(data[i]);
				const uint32_t body_cell_index(getBodyIndex(b));
				if (body_cell_index != index) {
					out.push_back(b);
					indexes.push_back(body_cell_index);
					removeBody(i--);
				}
			}
		}

		uint32_t getBodyIndex(const up::Body& b)
		{
			const uint32_t safety_offset(1U);
			const up::Vec2& v(b.position());
			int32_t body_x = uint32_t(v.x) + safety_offset;
			int32_t body_y = uint32_t(v.y) + safety_offset;

			return body_y + height * body_x;
		}

		const uint32_t cell_size;
		const uint32_t index;
		const uint32_t height;

		uint32_t data_size;
		uint32_t sides_size;
		std::vector<up::Body> data;
		std::vector<up::Body*> sides;
	};

	class AccessGrid
	{
	public:
		AccessGrid(const up::Vec2& dimension, uint32_t cell_size)
			: m_body_count(0U)
		{
			const uint32_t cell_count(0U)
			//for ()
		}

		~AccessGrid() = default;

		void addBody(const up::Body& body)
		{
			++m_body_count;

		}

	private:
		uint32_t m_body_count;
		std::vector<GridCell> m_cells;
	};
}


