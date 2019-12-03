#ifndef SRC_ENGINE_CLUSTER_H_
#define SRC_ENGINE_CLUSTER_H_

#include "board.h"

namespace go
{
namespace engine
{

uint32_t get_num_liberties(const Cluster&);
uint32_t get_cluster_idx(const ClusterTable& table, uint32_t cell_idx);
Cluster& get_cluster(ClusterTable& table, uint32_t cell_idx);
const Cluster& get_cluster(const ClusterTable& table, uint32_t cell_idx);

// Updates cluster information given an action.
void update_clusters(GameState&, const Action&);

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_CLUSTER_H_