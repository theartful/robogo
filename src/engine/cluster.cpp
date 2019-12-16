#include <algorithm>

#include "cluster.h"
#include "interface.h"
#include "liberties.h"
#include "utility.h"

using namespace go::engine;

static inline void
init_single_cell_cluster(Cluster&, const BoardState&, const Action&);
static inline Cluster* merge_clusters(Cluster**, uint32_t);
static inline void
merge_cluster_with_cell(Cluster&, uint32_t, ClusterTable&, const BoardState&);
static inline void capture_cluster(Cluster&, GameState&);

uint32_t
go::engine::get_cluster_idx(const ClusterTable& table, uint32_t cell_idx)
{
	uint32_t idx_crawler = cell_idx;
	while (table.clusters[idx_crawler].parent_idx != idx_crawler)
		idx_crawler = table.clusters[idx_crawler].parent_idx;

	// lazy update
	table.clusters[cell_idx].parent_idx = idx_crawler;
	return idx_crawler;
}

Cluster& go::engine::get_cluster(ClusterTable& table, uint32_t cell_idx)
{
	return table.clusters[get_cluster_idx(table, cell_idx)];
}

const Cluster&
go::engine::get_cluster(const ClusterTable& table, uint32_t cell_idx)
{
	return table.clusters[get_cluster_idx(table, cell_idx)];
}

static inline uint32_t compute_atari_lib(const Cluster& cluster)
{
	uint32_t first_set_bit = 0;
	for (; !cluster.liberties_map[first_set_bit]; first_set_bit++)
		;
	return first_set_bit;
}

static inline void
update_if_atari(const BoardState& state, Cluster& cluster, ClusterTable& table)
{
	if (in_atari(cluster))
	{
		table.num_in_atari++;
		for_each_liberty(state, cluster, [&](uint32_t pos) {
			cluster.atari_lib = pos;
			return BREAK;
		});
		// cluster.atari_lib = compute_atari_lib(cluster);
	}
}

uint32_t
go::engine::update_clusters(GameState& game_state, const Action& action)
{
	auto& table = game_state.cluster_table;
	auto& board_state = game_state.board_state;
	uint32_t num_captured_stones = 0;

	for_each_neighbor(board_state, action.pos, [&](auto idx) {
		increment_cell_count(
		    game_state.board_state, PLAYERS[game_state.player_turn], idx);
		decrement_empty_count(game_state.board_state, idx);
	});
	remove_empty_cell(board_state, action.pos);

	// obtain a list of neighbor clusters, and update liberties
	// of enemy clusters
	Cluster* to_merge[4];
	Cluster* to_capture[4];
	Cluster* enemy_clusters[4];
	uint32_t merge_count = 0;
	uint32_t capture_count = 0;
	uint32_t enemy_count = 0;

	for_each_neighbor_cluster(
	    table, board_state, action.pos, [&](auto& cluster) {
		    // if friendly cluster, add it to be merged
		    if (cluster.player == action.player_index)
		    {
			    to_merge[merge_count++] = &cluster;
			    if (cluster.num_liberties == 1)
				    table.num_in_atari--;
		    }
		    // if enemy cluster with zero liberties, add it to be captured
		    else
		    {
			    enemy_clusters[enemy_count++] = &cluster;
			    cluster.liberties_map.set(action.pos, false);
			    cluster.num_liberties--;
			    if (cluster.num_liberties == 0)
			    {
				    table.num_in_atari--;
				    to_capture[capture_count++] = &cluster;
				    num_captured_stones += cluster.size;
			    }
		    }
	    });

	Cluster* new_cluster;
	if (merge_count == 0)
	{
		Cluster& action_cluster = table.clusters[action.pos];
		init_single_cell_cluster(action_cluster, board_state, action);
		new_cluster = &action_cluster;
	}
	else
	{
		Cluster& mega_cluster = *merge_clusters(to_merge, merge_count);
		merge_cluster_with_cell(mega_cluster, action.pos, table, board_state);
		new_cluster = &mega_cluster;
	}
	// now cleanup dead clusters
	for (auto it = to_capture; it != to_capture + capture_count; it++)
		capture_cluster(**it, game_state);

	// update atari status
	for (auto it = enemy_clusters; it != enemy_clusters + enemy_count; it++)
		update_if_atari(board_state, **it, table);
	update_if_atari(board_state, *new_cluster, table);

	return num_captured_stones;
}

static inline void init_single_cell_cluster(
    Cluster& cluster, const BoardState& state, const Action& action)
{
	cluster.player = action.player_index;
	cluster.parent_idx = action.pos;
	cluster.size = 1;
	cluster.liberties_map.reset();
	cluster.num_liberties = 0;
	for_each_neighbor(state, action.pos, [&](uint32_t neighbor) {
		if (is_empty_cell(state, neighbor))
		{
			cluster.liberties_map.set(neighbor, true);
			cluster.num_liberties++;
		}
	});
	// table.next_cell[action.pos] = action.pos;
	// cluster.tail = action.pos;
}

static inline void merge_cluster_with_cell(
    Cluster& cluster, uint32_t cell_index, ClusterTable& table,
    const BoardState& state)
{
	Cluster& cell_cluster = table.clusters[cell_index];
	cell_cluster.parent_idx = cluster.parent_idx;
	// add the effects of the single cell cluster:
	//     1. Remove the liberty where it's played
	//     2. Add its own liberties
	for_each_neighbor(state, cell_index, [&](uint32_t neighbor) {
		if (is_empty_cell(state, neighbor))
			cluster.liberties_map.set(neighbor, true);
	});
	cluster.liberties_map.set(cell_index, false);

	cluster.num_liberties = cluster.liberties_map.count();
	cluster.size++;

	// table.next_cell[cluster.tail] = cell_index;
	// table.next_cell[cell_index] = cluster.parent_idx;
	// cluster.tail = cell_index;
}

static inline Cluster* merge_clusters(Cluster* clusters[], uint32_t count)
{
	if (count == 1)
		return clusters[0];
	// find cluster with maximum size and make it the first element
	std::swap(
	    *std::max_element(
	        clusters, clusters + count,
	        [](Cluster* a, Cluster* b) { return a->size < b->size; }),
	    *clusters);

	Cluster* biggest = clusters[0];
	for (auto it = clusters + 1; it != clusters + count; it++)
	{
		Cluster* to_merge = *it;

		// table.next_cell[biggest->tail] = to_merge->parent_idx;
		// table.next_cell[to_merge->tail] = biggest->parent_idx;
		// biggest->tail = to_merge->tail;

		biggest->size += to_merge->size;
		to_merge->parent_idx = biggest->parent_idx;
		biggest->liberties_map |= to_merge->liberties_map;
	}
	return biggest;
}

static inline void capture_cluster(Cluster& cluster, GameState& game_state)
{
	auto& board_state = game_state.board_state;
	auto& table = game_state.cluster_table;

	// update player info
	auto captured_player_idx = cluster.player;
	auto& captured_player = game_state.players[captured_player_idx];
	captured_player.number_alive_stones -= cluster.size;
	auto& other_player = game_state.players[1 - captured_player_idx];
	other_player.number_captured_enemies += cluster.size;

	for_each_cluster_cell(cluster, board_state, [&](uint32_t cell_idx) {
		for_each_neighbor_cluster(
		    table, board_state, cell_idx, [&](Cluster& neighbor) {
			    if (&neighbor != &cluster)
			    {
				    neighbor.liberties_map.set(cell_idx, true);
				    neighbor.num_liberties++;
			    }
		    });
		for_each_neighbor(board_state, cell_idx, [&](uint32_t neighbor) {
			increment_empty_count(board_state, neighbor);
			decrement_cell_count(
			    board_state, PLAYERS[cluster.player], neighbor);
		});
		board_state.board[cell_idx] = Cell::EMPTY;
		add_empty_cell(board_state, cell_idx);
	});

	cluster.size = 0;
}

uint32_t go::engine::get_num_liberties(const Cluster& cluster)
{
	return cluster.num_liberties;
}
