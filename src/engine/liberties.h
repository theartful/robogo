#ifndef SRC_ENGINE_LIBERTIES_H_
#define SRC_ENGINE_LIBERTIES_H_

#include "utility.h"
#include <stdint.h>
namespace go
{
namespace engine
{
struct BoardState;
struct ClusterTable;

// Slowest and most basic liberty counting function that doesn't depend
// on cached data
uint32_t count_liberties(const BoardState& state, uint32_t i, uint32_t j);
uint32_t count_liberties(const BoardState& state, uint32_t cell_idx);

// Finds liberty count from cached data
uint32_t count_liberties(const ClusterTable& table, uint32_t i, uint32_t j);
uint32_t count_liberties(const ClusterTable& table, uint32_t cell_idx);

inline bool in_atari(const Cluster& cluster)
{
	return cluster.num_liberties == 1;
}
inline bool in_atari(const GameState& game_state, uint32_t pos)
{
	return count_liberties(game_state.cluster_table, pos) == 1;
}
inline uint32_t get_atari_lib(const Cluster& cluster)
{
	return cluster.atari_lib;
}

inline bool is_cluster_lib(const Cluster& cluster, uint32_t lib)
{
	return cluster.liberties_map[lib];
}
inline bool will_be_surrounded(const GameState& state, uint32_t pos)
{
	auto& board = state.board_state;
	if (get_empty_count(board, pos) > 1)
		return false;
	if (get_white_count(board, pos) > 1 && get_black_count(board, pos) > 1)
		return false;

	bool atari_exists = false;
	for_each_neighbor(board, pos, [&](auto neighbor) {
		if (in_atari(state, neighbor))
			atari_exists = true;
	});
	return !atari_exists;
}

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_LIBERTIES_H_