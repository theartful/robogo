#include <array>

#include "board.h"
#include "engine.h"
#include "group.h"
#include "iterators.h"
#include "liberties.h"

using go::engine::BoardState;
using go::engine::Stone;

uint32_t
go::engine::count_liberties(const BoardState& state, uint32_t stone_idx)
{
	uint32_t num_liberties = 0;
	for_each_stone(state, stone_idx, [&](uint32_t cur_idx) {
		if (is_empty(state, cur_idx))
			num_liberties++;
		if (state.stones[cur_idx] == state.stones[stone_idx])
			return Expand;
		return DontExpand;
	});
	return num_liberties;
}

uint32_t
go::engine::count_liberties(const GroupTable& table, uint32_t stone_idx)
{
	return get_num_liberties(get_group(table, stone_idx));
}
