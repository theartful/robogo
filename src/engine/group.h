#ifndef SRC_ENGINE_CLUSTER_H_
#define SRC_ENGINE_CLUSTER_H_

#include "game.h"

namespace go::engine
{

struct GameState;

// A group is a maximal set of connected stones
struct Group
{
	uint16_t parent : 15;
	uint16_t player_idx : 1;
	uint16_t size;
	uint16_t num_libs;
	uint16_t atari_lib;
	MarginRemapped2DBitset<BoardState::MAX_SIZE> lib_map;

	constexpr Group() :
		parent{0}, player_idx{0}, size{0}, num_libs{0}, atari_lib{0}, lib_map{}
	{
	}
};

// A union find structure
struct GroupTable
{
	// checkout https://senseis.xmp.net/?MaximumNumberOfLiveGroups
	static constexpr uint32_t MAX_GROUP_NUM = 277;

	std::array<Group, MAX_GROUP_NUM> groups;
	mutable MarginRemapped2DArray<int16_t, BoardState::MAX_SIZE> parents;

	uint16_t max_group_idx;
	uint16_t num_in_atari;

	// TODO: decide whether to use linked structure to quickly iterate group
	// stones or leave the current implementation
	// mutable std::array<uint16_t, BoardState::EXTENDED_AREA> next_stone;

	constexpr GroupTable() :
		groups{}, parents{}, max_group_idx{0}, num_in_atari{0}
	{
	}
};

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
