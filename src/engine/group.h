#ifndef SRC_ENGINE_CLUSTER_H_
#define SRC_ENGINE_CLUSTER_H_

#include "board.h"

namespace go::engine
{

uint32_t get_group_idx(const GroupTable&, uint32_t);
Group& get_group(GroupTable&, uint32_t);
const Group& get_group(const GroupTable&, uint32_t);

// Updates group information given an action.
// returns number of captured stones
uint32_t update_groups(GameState&, const Action&);

inline constexpr auto get_group_parent(const Group& group)
{
	return group.parent;
}
inline constexpr auto get_group_size(const Group& group)
{
	return group.size;
}
inline constexpr auto get_num_liberties(const Group& group)
{
	return group.num_libs;
}

} // namespace go::engine

#endif // SRC_ENGINE_CLUSTER_H_