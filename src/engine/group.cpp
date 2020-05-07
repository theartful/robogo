#include <algorithm>

#include "engine.h"
#include "game.h"
#include "group.h"
#include "liberties.h"
#include "utility.h"

using namespace go::engine;

static inline uint32_t init_single_stone_group(
    GroupTable&, uint16_t, const BoardState&, const Action&);
static inline uint32_t merge_groups(GroupTable&, uint32_t*, uint32_t);
static inline void
merge_group_with_stone(uint16_t, uint32_t, GroupTable&, const BoardState&);
static inline void capture_group(Group&, GameState&);

static inline bool is_parent_stone(const GroupTable& table, uint32_t idx)
{
	// this assumes that 0 is not a valid stone index
	return table.parents[idx] > 0;
}
static inline void
set_stone_parent(const GroupTable& table, uint32_t idx, uint32_t stone_idx)
{
	table.parents[idx] = stone_idx;
}
static inline void
set_group_parent(const GroupTable& table, uint32_t idx, uint32_t group_idx)
{
	table.parents[idx] = -group_idx;
}
static inline uint32_t get_parent(const GroupTable& table, uint32_t idx)
{
	return static_cast<uint32_t>(std::abs(table.parents[idx]));
}

uint32_t go::engine::get_group_idx(const GroupTable& table, uint32_t stone_idx)
{
	uint16_t idx_crawler = stone_idx;
	while (is_parent_stone(table, idx_crawler))
		idx_crawler = get_parent(table, idx_crawler);
	uint16_t group_parent = idx_crawler;

	// lazy update
	idx_crawler = stone_idx;
	while (is_parent_stone(table, idx_crawler))
	{
		auto parent = get_parent(table, idx_crawler);
		set_stone_parent(table, idx_crawler, group_parent);
		idx_crawler = parent;
	}

	return get_parent(table, idx_crawler);
}

Group& go::engine::get_group(GroupTable& table, uint32_t stone_idx)
{
	return table.groups[get_group_idx(table, stone_idx)];
}

const Group& go::engine::get_group(const GroupTable& table, uint32_t stone_idx)
{
	return table.groups[get_group_idx(table, stone_idx)];
}

static inline uint32_t compute_atari_lib(const Group& group)
{
	uint32_t first_set_bit = 0;
	for (; !group.lib_map[first_set_bit]; first_set_bit++)
		;
	return first_set_bit;
}

static inline void
update_if_atari(const BoardState& state, Group& group, GroupTable& table)
{
	if (in_atari(group))
	{
		table.num_in_atari++;
		for_each_liberty(state, group, [&](uint32_t pos) {
			group.atari_lib = pos;
			return Break;
		});
	}
}

static inline uint32_t find_new_group(GroupTable& table)
{
	auto find_new_group_ = [&table]() {
		for (; table.max_group_idx < table.groups.size(); table.max_group_idx++)
			if (table.groups[table.max_group_idx].size == 0)
				return true;
		return false;
	};
	if (find_new_group_())
		return table.max_group_idx++;
	table.max_group_idx = 0;
	if (find_new_group_())
		return table.max_group_idx++;

	DEBUG_PRINT(
	    "engine::new_group: couldn't create new group. this shouldn't happen!");
	exit(-1);
}

uint32_t go::engine::update_groups(GameState& game_state, const Action& action)
{
	auto& table = game_state.group_table;
	auto& board = game_state.board;
	uint32_t num_captured_stones = 0;

	for_each_neighbor(board, action.pos, [&](auto idx) {
		increment_neighbors(game_state.board, game_state.player_turn, idx);
		decrement_empty_neighbors(game_state.board, idx);
	});
	remove_empty_stone(board, action.pos);

	// obtain a list of neighbor groups, and update liberties
	// of enemy groups
	uint32_t to_merge[4];
	uint32_t to_capture[4];
	uint32_t merge_count = 0;
	uint32_t capture_count = 0;

	for_each_neighbor_group(
	    table, board, action.pos, [&](auto group_idx, auto& group) {
		    // if friendly group, add it to be merged
		    if (group.player_idx == action.player_idx)
		    {
			    to_merge[merge_count++] = group_idx;
			    if (in_atari(group))
				    table.num_in_atari--;
		    }
		    // if enemy group with zero liberties, add it to be captured
		    else
		    {
			    if (in_atari(group))
			    {
				    table.num_in_atari--;
				    to_capture[capture_count++] = group_idx;
				    num_captured_stones += group.size;
			    }
			    else
			    {
				    group.num_libs--;
				    group.lib_map.reset(action.pos);
				    update_if_atari(board, group, table);
			    }
		    }
	    });

	uint32_t new_group_idx;
	if (merge_count == 0)
	{
		new_group_idx = init_single_stone_group(
		    table, find_new_group(table), board, action);
	}
	else
	{
		uint32_t mega_group_idx = merge_groups(table, to_merge, merge_count);
		merge_group_with_stone(mega_group_idx, action.pos, table, board);
		new_group_idx = mega_group_idx;
	}
	// now cleanup dead groups
	for (auto it = to_capture; it != to_capture + capture_count; it++)
		capture_group(table.groups[*it], game_state);

	update_if_atari(board, table.groups[new_group_idx], table);
	return num_captured_stones;
}

static inline uint32_t init_single_stone_group(
    GroupTable& table, uint16_t group_idx, const BoardState& state,
    const Action& action)
{
	set_group_parent(table, action.pos, group_idx);
	auto& group = table.groups[group_idx];
	group.player_idx = action.player_idx;
	group.parent = action.pos;
	group.size = 1;
	group.lib_map.reset();
	group.num_libs = 0;
	for_each_neighbor(state, action.pos, [&](uint32_t neighbor) {
		if (is_empty(state, neighbor))
		{
			group.lib_map.set(neighbor);
			group.num_libs++;
		}
	});
	// table.next_stone[action.pos] = action.pos;
	// group.tail = action.pos;
	return group_idx;
}

static inline void merge_group_with_stone(
    uint16_t group_idx, uint32_t stone_idx, GroupTable& table,
    const BoardState& state)
{
	Group& group = table.groups[group_idx];
	set_stone_parent(table, stone_idx, group.parent);
	// add the effects of the single stone group:
	//     1. Remove the liberty where it's played
	//     2. Add its own liberties
	for_each_neighbor(state, stone_idx, [&](uint32_t neighbor) {
		if (is_empty(state, neighbor))
		{
			if (!group.lib_map[neighbor])
			{
				group.num_libs++;
				group.lib_map.set(neighbor);
			}
		}
	});
	group.lib_map.reset(stone_idx);
	group.num_libs--;
	group.size++;

	// table.next_stone[group.tail] = stone_index;
	// table.next_stone[stone_index] = group.parent;
	// group.tail = stone_index;
}

static inline uint32_t
merge_groups(GroupTable& table, uint32_t* groups_idxs, uint32_t count)
{
	if (count == 1)
		return groups_idxs[0];
	// find group with maximum size and make it the first element
	std::swap(
	    *std::max_element(
	        groups_idxs, groups_idxs + count,
	        [&](uint32_t a, uint32_t b) {
		        return table.groups[a].size < table.groups[b].size;
	        }),
	    *groups_idxs);

	uint32_t biggest = groups_idxs[0];
	Group& biggest_group = table.groups[biggest];
	for (auto it = groups_idxs + 1; it != groups_idxs + count; it++)
	{
		Group& to_merge = table.groups[*it];

		// table.next_stone[biggest->tail] = to_merge->parent;
		// table.next_stone[to_merge->tail] = biggest->parent;
		// biggest->tail = to_merge->tail;

		biggest_group.size += to_merge.size;
		set_stone_parent(table, to_merge.parent, biggest_group.parent);
		biggest_group.lib_map |= to_merge.lib_map;
		to_merge.size = 0;
	}
	return biggest;
}

static inline void capture_group(Group& group, GameState& game)
{
	auto& board = game.board;
	auto& table = game.group_table;

	// update player info
	game.players[group.player_idx].num_alive -= group.size;
	game.players[1u - group.player_idx].num_captures += group.size;

	for_each_group_stone(group, board, [&](uint32_t stone_idx) {
		for_each_neighbor_group(table, board, stone_idx, [&](Group& neighbor) {
			if (&neighbor != &group)
			{
				neighbor.lib_map.set(stone_idx);
				neighbor.num_libs++;
			}
		});
		for_each_neighbor(board, stone_idx, [&](uint32_t neighbor) {
			increment_empty_neighbors(board, neighbor);
			decrement_neighbors(board, group.player_idx, neighbor);
		});
		board.stones[stone_idx] = Stone::Empty;
		add_empty_stone(board, stone_idx);
	});

	group.size = 0;
}
