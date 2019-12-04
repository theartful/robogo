#include <array>

#include "board.h"
#include "cluster.h"
#include "interface.h"
#include "liberties.h"
#include "utility.h"

using go::engine::BoardState;
using go::engine::Cell;

uint32_t go::engine::count_liberties(const BoardState& state, uint32_t cell_idx)
{
	uint32_t num_liberties = 0;
	for_each_cell(state, cell_idx, [&](uint32_t cur_idx) {
		if (is_empty_cell(state, cur_idx))
			num_liberties++;
		if (state.board[cur_idx] == state.board[cell_idx])
			return EXPAND;
		return DONT_EXPAND;
	});
	return num_liberties;
}

uint32_t
go::engine::count_liberties(const BoardState& state, uint32_t i, uint32_t j)
{
	return count_liberties(state, BoardState::index(i, j));
}

uint32_t
go::engine::count_liberties(const ClusterTable& table, uint32_t i, uint32_t j)
{
	return count_liberties(table, BoardState::index(i, j));
}

uint32_t
go::engine::count_liberties(const ClusterTable& table, uint32_t cell_idx)
{
	return get_num_liberties(get_cluster(table, cell_idx));
}
