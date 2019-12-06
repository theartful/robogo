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

// not accurate. does not handle enemy captures well
inline bool is_self_atari(const GameState& game_state, uint32_t pos)
{
	auto& table = game_state.cluster_table;
	auto& board = game_state.board_state;
	std::bitset<BoardState::MAX_NUM_CELLS> lib_map;
	uint32_t empty_count = 0;
	for_each_neighbor(board, pos, [&](uint32_t neighbor) {
		if (is_empty_cell(board, neighbor))
		{
			empty_count++;
			lib_map.set(neighbor);
		}
	});
	if (empty_count > 1)
		return false;

	uint32_t capture = 0;
	for_each_neighbor_cluster(table, board, pos, [&](auto& cluster) {
		if (cluster.player == game_state.player_turn)
			lib_map |= cluster.liberties_map;
		else if (in_atari(cluster))
			capture++;
	});
	return (lib_map.count() + capture) <= 2;
}
inline bool is_cluster_lib(const Cluster& cluster, uint32_t lib)
{
	return cluster.liberties_map[lib];
}
inline bool is_surrounded(const BoardState& state, uint32_t pos)
{
	unsigned char color = 0;
	for_each_neighbor(state, pos, [&](uint32_t neighbor) {
		if (state.board[neighbor] == Cell::EMPTY)
			// set color to any invalid value
			color = static_cast<unsigned char>(Cell::BORDER);
		else
			color |= static_cast<unsigned char>(state.board[neighbor]);
	});
	return color == static_cast<unsigned char>(Cell::WHITE) ||
	       color == static_cast<unsigned char>(Cell::BLACK);
}

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_LIBERTIES_H_