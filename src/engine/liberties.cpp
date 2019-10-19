#include <array>
#include <stack>

#include "liberties.h"
#include "board.h"
#include "interface.h"

using go::engine::BoardState;
using go::engine::Cell;

static constexpr size_t VISITED_CACHE_SIZE =
    BoardState::MAX_BOARD_SIZE * BoardState::MAX_BOARD_SIZE;

uint32_t count_liberties(const BoardState& state, uint32_t i, uint32_t j)
{
	uint32_t pos = BoardState::index(i, j);
	Cell cell = state.board[pos];

	assert(!is_empty_cell(cell));
	if (is_dead_cell(cell))
		return 0;

	uint32_t num_liberties = 0;
	std::array<bool, VISITED_CACHE_SIZE> visited = {};
	std::stack<uint32_t> to_visit;
	to_visit.push(pos);

	auto visit_pos = [&](uint32_t new_pos) -> void {
		if (is_empty_cell(state.board[new_pos]))
		{
			num_liberties++;
		}
		else if (!visited[pos] && state.board[new_pos] == cell)
		{
			visited[new_pos] = true;
			to_visit.push(new_pos);
		}
	};

	while(!to_visit.empty())
	{
		uint32_t cur_pos = to_visit.top();
		to_visit.pop();

		if (int32_t up = pos - BoardState::MAX_BOARD_SIZE; up >= 0)
			visit_pos(up);

		if (int32_t down = pos + BoardState::MAX_BOARD_SIZE;
			down <= VISITED_CACHE_SIZE)
			visit_pos(down);

		if (int32_t left = pos - 1; left >= 0)
			visit_pos(left);

		if (int32_t right = pos + 1; right <= VISITED_CACHE_SIZE)
			visit_pos(right);
	}

	return num_liberties;
}
