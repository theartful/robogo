#include <array>

#include "board.h"
#include "interface.h"
#include "liberties.h"

using go::engine::BoardState;
using go::engine::Cell;

// For the DFS, we need a stack of nodes to visit, and hashtable for visited
// nodes. This class does both things efficiently without memory allocation
class count_liberties_cache
{
public:
	static constexpr uint32_t VISIT_BIT = 1U << 31;
	bool empty() const
	{
		return top_index == -1;
	}
	uint32_t top() const
	{
		assert(top_index >= 0);
		return cache[top_index] & (~VISIT_BIT);
	}
	void pop()
	{
		assert(top_index >= 0);
		top_index--;
	}
	void push(uint32_t value)
	{
		top_index++;
		// clear all except VISIT_BIT
		cache[top_index] &= VISIT_BIT;
		// store the value in the first 31 bits;
		cache[top_index] |= value;
	}
	bool is_visited(uint32_t index) const
	{
		return (cache[index] & VISIT_BIT) == VISIT_BIT;
	}
	void mark_visited(uint32_t index)
	{
		cache[index] |= VISIT_BIT;
	}

private:
	std::array<uint32_t, BoardState::MAX_NUM_CELLS> cache = {};
	uint32_t top_index = -1;
};

uint32_t count_liberties(const BoardState& state, uint32_t i, uint32_t j)
{
	uint32_t pos = BoardState::index(i, j);
	Cell cell = state.board[pos];

	// shouldn't be called on an empty cell
	assert(!is_empty_cell(cell));
	if (is_dead_cell(cell))
		return 0;

	uint32_t num_liberties = 0;
	count_liberties_cache cache_stack;
	cache_stack.push(pos);

	auto visit_pos = [&](uint32_t new_pos) -> void {
		if (is_empty_cell(state.board[new_pos]))
		{
			num_liberties++;
		}
		else if (!cache_stack.is_visited(pos) && state.board[new_pos] == cell)
		{
			cache_stack.mark_visited(new_pos);
			cache_stack.push(new_pos);
		}
	};

	while (!cache_stack.empty())
	{
		uint32_t cur_pos = cache_stack.top();
		cache_stack.pop();

		std::array<int32_t, 4> neighbours = {pos - BoardState::MAX_BOARD_SIZE,
		                                     pos + BoardState::MAX_BOARD_SIZE,
		                                     pos - 1, pos + 1};

		for (int32_t neighbour : neighbours)
			if (neighbour >= 0 && neighbour < BoardState::MAX_NUM_CELLS)
				visit_pos(neighbour);
	}

	return num_liberties;
}
