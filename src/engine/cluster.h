#ifndef SRC_ENGINE_CLUSTER_H_
#define SRC_ENGINE_CLUSTER_H_

#include <array>
#include <bitset>
#include <stdint.h>

#include "board.h"

namespace go
{
namespace engine
{

// A cluster is a maximal set of connected stones
struct Cluster
{
	mutable uint32_t parent_idx;
	uint32_t player;
	uint32_t size;
	uint32_t num_liberties;
	std::bitset<BoardState::MAX_NUM_CELLS> liberties_map;
};

// A union find structure
struct ClusterTable
{
	std::array<Cluster, BoardState::MAX_NUM_CELLS> clusters;
	ClusterTable() : clusters{} // initialize clusters to 0
	{
	}
};

uint32_t get_num_liberties(const Cluster&);
uint32_t get_cluster_idx(const ClusterTable& table, uint32_t cell_idx);
Cluster& get_cluster(ClusterTable& table, uint32_t cell_idx);
const Cluster& get_cluster(const ClusterTable& table, uint32_t cell_idx);

// Updates cluster information given an action.
void update_clusters(ClusterTable&, BoardState&, const Action&);

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_CLUSTER_H_