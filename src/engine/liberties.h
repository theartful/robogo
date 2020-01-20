#ifndef _ENGINE_LIBERTIES_H_
#define _ENGINE_LIBERTIES_H_

#include "iterators.h"
#include <stdint.h>
namespace go::engine
{

struct BoardState;
struct GroupTable;

// Slowest and most basic liberty counting function that doesn't depend
// on cached data
uint32_t count_liberties(const BoardState&, uint32_t);

// Finds liberty count from cached data
uint32_t count_liberties(const GroupTable&, uint32_t);

inline bool in_atari(const Group& group)
{
	return group.num_libs == 1;
}
inline bool in_atari(const GameState& game_state, uint32_t pos)
{
	return count_liberties(game_state.group_table, pos) == 1;
}

inline uint32_t get_atari_lib(const Group& group)
{
	return group.atari_lib;
}

inline bool is_group_lib(const Group& group, uint32_t lib)
{
	return group.lib_map[lib];
}
inline bool will_be_surrounded(const GameState& state, uint32_t pos)
{
	auto& board = state.board;
	if (get_empty_neighbors_count(board, pos) >= 1)
		return false;
	if (get_white_neighbors_count(board, pos) >= 1 &&
	    get_black_neighbors_count(board, pos) >= 1)
		return false;

	bool atari_exists = false;
	for_each_neighbor(board, pos, [&](auto neighbor) {
		if (in_atari(state, neighbor))
			atari_exists = true;
	});
	return !atari_exists;
}

} // namespace go::engine

#endif // _ENGINE_LIBERTIES_H_