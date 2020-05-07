#include <array>

#include "board.h"
#include "engine.h"
#include "group.h"
#include "iterators.h"
#include "liberties.h"

namespace go::engine
{

uint32_t count_liberties(const BoardState& state, uint32_t stone_idx)
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

uint32_t count_liberties(const GroupTable& table, uint32_t stone_idx)
{
	return get_num_liberties(get_group(table, stone_idx));
}

uint32_t count_liberties(const GameState& game, uint32_t stone_idx)
{
	return count_liberties(game.group_table, stone_idx);
}

std::vector<uint32_t> get_liberties(const BoardState& state, const Group& group)
{
	std::vector<uint32_t> vec;
	for_each_liberty(state, group, [&] (uint32_t idx) {
		vec.push_back(idx);
	});
	return vec;
}

std::vector<uint32_t> get_liberties(const GameState& game, uint32_t stone_idx)
{
	return get_liberties(game.board, get_group(game.group_table, stone_idx));
}
}
