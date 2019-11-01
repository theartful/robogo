#include <algorithm>

#include "cluster.h"
#include "interface.h"
#include "liberties.h"
#include "utility.h"

using namespace go::engine;

static void
init_single_cell_cluster(Cluster&, const BoardState&, const Action&);
static Cluster* merge_clusters(Cluster**, uint32_t);
static void
merge_cluster_with_cell(Cluster&, uint32_t, ClusterTable&, const BoardState&);
static void capture_cluster(Cluster&, ClusterTable&, BoardState&);

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

void go::engine::update_clusters(
    ClusterTable& table, BoardState& state, const Action& action)
{
	// obtain a list of neighbor clusters, and update liberties
	// of enemy clusters
	Cluster* to_merge[4];
	Cluster* to_capture[4];
	uint32_t merge_count = 0;
	uint32_t capture_count = 0;

	for_each_neighbor_cluster(table, state, action.pos, [&](Cluster& cluster) {
		cluster.liberties_map.set(action.pos, false);
		cluster.num_liberties--;
		// if friendly cluster, add it to be merged
		if (cluster.player == action.player_index)
			to_merge[merge_count++] = &cluster;
		// if enemy cluster with zero liberties, add it to be captured
		else if (cluster.num_liberties == 0)
			to_capture[capture_count++] = &cluster;
	});

	Cluster& action_cluster = table.clusters[action.pos];
	if (merge_count == 0)
	{
		init_single_cell_cluster(action_cluster, state, action);
	}
	else
	{
		Cluster& mega_cluster = *merge_clusters(to_merge, merge_count);
		merge_cluster_with_cell(mega_cluster, action.pos, table, state);
	}
	// now cleanup dead clusters
	for (auto it = to_capture; it != to_capture + capture_count; it++)
		capture_cluster(**it, table, state);
}

static void init_single_cell_cluster(
    Cluster& cluster, const BoardState& state, const Action& action)
{
	cluster.parent_idx = action.pos;
	cluster.player = action.player_index;
	cluster.size = 1;
	cluster.liberties_map.reset();
	cluster.num_liberties = 0;
	for_each_neighbor(action.pos, [&](uint32_t neighbor) {
		if (is_empty_cell(state, neighbor))
		{
			cluster.liberties_map.set(neighbor, true);
			cluster.num_liberties++;
		}
	});
}

static void merge_cluster_with_cell(
    Cluster& cluster, uint32_t cell_index, ClusterTable& table,
    const BoardState& state)
{
	Cluster& cell_cluster = table.clusters[cell_index];
	cell_cluster.parent_idx = cluster.parent_idx;
	// add the effects of the single cell cluster:
	//     1. Remove the liberty where it's played
	//     2. Add its own liberties
	cluster.liberties_map.set(cell_index, false);
	for_each_neighbor(cell_index, [&](uint32_t neighbor) {
		if (is_empty_cell(state, neighbor))
			cluster.liberties_map.set(neighbor, true);
	});
	cluster.num_liberties = cluster.liberties_map.count();
	cluster.size++;
}

static Cluster* merge_clusters(Cluster* clusters[], uint32_t count)
{
	assert(count >= 1);
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
		biggest->size += to_merge->size;
		to_merge->parent_idx = biggest->parent_idx;
		biggest->liberties_map |= to_merge->liberties_map;
	}
	return biggest;
}

static void
capture_cluster(Cluster& cluster, ClusterTable& table, BoardState& state)
{
	for_each_cluster_cell(cluster, state, [&](uint32_t cell_idx) {
		for_each_neighbor_cluster(
		    table, state, cell_idx, [&](Cluster& neighbor) {
			    if (&neighbor != &cluster)
			    {
				    neighbor.liberties_map.set(cell_idx, true);
				    neighbor.num_liberties++;
			    }
		    });
		state.board[cell_idx] = Cell::EMPTY;
	});
}

uint32_t go::engine::get_num_liberties(const Cluster& cluster)
{
	return cluster.num_liberties;
}
