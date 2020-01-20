#ifndef _ENGINE_ITERATORS_H_
#define _ENGINE_ITERATORS_H_

#include <algorithm>
#include <array>
#include <type_traits>

#include "engine/board.h"
#include "engine/engine.h"
#include "engine/group.h"

namespace go::engine
{

enum ForPolicy
{
	Expand,
	DontExpand,
	Continue,
	Break,
};

namespace details
{
template <typename... Args, typename Lambda>
constexpr auto
wrap_void_lambda(Lambda&& lambda, ForPolicy default_policy = Continue)
{
	using namespace std;
	if constexpr (!is_same_v<std::invoke_result_t<Lambda, Args...>, void>)
		return lambda;
	else
		return [&lambda, default_policy](auto&&... val) {
			lambda(val...);
			return default_policy;
		};
}
} // namespace details

template <typename Lambda>
void for_each_neighbor(
    const BoardState& state, uint32_t stone_idx, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda<uint32_t>(std::forward<Lambda>(lambda));
	// right
	if (uint32_t right = stone_idx + 1; state.stones[right] != Stone::Border)
		if (wrapped_lambda(right) == Break)
			return;
	// up
	if (uint32_t up = stone_idx - BoardState::EXTENDED_SIZE;
	    state.stones[up] != Stone::Border)
		if (wrapped_lambda(up) == Break)
			return;
	// left
	if (uint32_t left = stone_idx - 1; state.stones[left] != Stone::Border)
		if (wrapped_lambda(left) == Break)
			return;
	// down
	if (uint32_t down = stone_idx + BoardState::EXTENDED_SIZE;
	    state.stones[down] != Stone::Border)
		if (wrapped_lambda(down) == Break)
			return;
}

template <typename Lambda>
void for_each_8neighbor_all(uint32_t pos, Lambda&& lambda)
{
	const auto neighbors = {
	    pos - BoardState::EXTENDED_SIZE - 1,
	    pos - BoardState::EXTENDED_SIZE,
	    pos - BoardState::EXTENDED_SIZE + 1,
	    pos - 1,
	    pos + 1,
	    pos + BoardState::EXTENDED_SIZE - 1,
	    pos + BoardState::EXTENDED_SIZE,
	    pos + BoardState::EXTENDED_SIZE + 1,
	};
	for (auto neighbor : neighbors)
		lambda(neighbor);
}

template <typename Lambda, typename CVGroupTable>
void for_each_neighbor_group(
    CVGroupTable& table, const BoardState& state, uint32_t stone_idx,
    Lambda&& lambda)
{
	constexpr bool pass_index =
	    std::is_invocable<Lambda, uint32_t, Group&>::value;
	auto get_wrapped_lambda = [&]() {
		if constexpr (pass_index)
			return details::wrap_void_lambda<uint32_t, Group&>(
			    std::forward<Lambda>(lambda));
		else
			return details::wrap_void_lambda<Group&>(
			    std::forward<Lambda>(lambda));
	};
	auto wrapped_lambda = get_wrapped_lambda();

	uint32_t visited[4];
	uint32_t visited_count = 0;
	for_each_neighbor(state, stone_idx, [&](uint32_t neighbor) {
		if (!is_empty(state.stones[neighbor]))
		{
			uint32_t group_idx = get_group_idx(table, neighbor);
			// check if visited
			const auto begin_it = visited;
			const auto end_it = visited + visited_count;
			if (std::find(begin_it, end_it, group_idx) == end_it)
			{
				visited[visited_count++] = group_idx;
				if constexpr (pass_index)
					return wrapped_lambda(group_idx, table.groups[group_idx]);
				else
					return wrapped_lambda(table.groups[group_idx]);
			}
		}
		return Continue;
	});
}

namespace details
{
struct SearchCache
{
	bool empty() const
	{
		return top_index == 0;
	}
	uint32_t pop()
	{
		return static_cast<uint32_t>(cache[--top_index] & (~VISIT_BIT));
	}
	void push(uint32_t value)
	{
		// clear all except VISIT_BIT
		cache[top_index] &= VISIT_BIT;
		// store the value in the first 31 bits;
		cache[top_index] |= value;
		top_index++;
	}
	bool is_visited(uint32_t index) const
	{
		return (cache[index] & VISIT_BIT);
	}
	void mark_visited(uint32_t index)
	{
		cache[index] |= VISIT_BIT;
	}
	void mark_unvisited(uint32_t index)
	{
		cache[index] &= ~(VISIT_BIT);
	}

private:
	static constexpr uint16_t VISIT_BIT = 1u << 15;
	std::array<uint16_t, BoardState::EXTENDED_AREA> cache = {};
	size_t top_index = 0;
};
} // namespace details

template <typename Lambda>
void for_each_stone(const BoardState& state, uint32_t root, Lambda&& lambda)
{
	details::SearchCache search_cache;
	search_cache.push(root);
	search_cache.mark_visited(root);

	while (!search_cache.empty())
	{
		uint32_t cur_pos = search_cache.pop();
		if (lambda(cur_pos) == Expand)
		{
			for_each_neighbor(state, cur_pos, [&](uint32_t neighbour) {
				if (!search_cache.is_visited(neighbour))
				{
					search_cache.push(neighbour);
					search_cache.mark_visited(neighbour);
				}
			});
		}
	}
}

template <typename Lambda>
void for_each_group_stone(
    const Group& group, const BoardState& state, Lambda&& lambda)
{
	auto wrapped_lambda = details::wrap_void_lambda<uint32_t>(
	    std::forward<Lambda>(lambda), Expand);
	Stone group_color = state.stones[group.parent];
	for_each_stone(state, group.parent, [&](uint32_t idx) {
		if (state.stones[idx] == group_color)
			return wrapped_lambda(idx);
		else
			return DontExpand;
	});
}

/*
template <typename Lambda>
void for_each_group_stone(
    const Group& group, const GroupTable& table, Lambda&& lambda)
{
    auto wrapped_lambda = details::wrap_void_lambda<uint32_t, Expand>(
        std::forward<Lambda>(lambda));

    uint32_t index = group.parent;
    do
    {
        if (wrapped_lambda(index) == Break)
            return;
        index = table.next_stone[index];
    } while (index != group.parent);
}
*/

template <typename Lambda>
void for_each_empty_stone(const BoardState& state, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda<uint32_t>(std::forward<Lambda>(lambda));
	for (uint32_t i = 0; i < state.num_empty_stones; i++)
	{
		if (wrapped_lambda(state.empty_stones[i]) == Break)
			return;
	}
}

// if there are no actions, calls lambda on pass, otherwise pass is not
// considered
template <typename Lambda>
void for_each_legal_action(const GameState& state, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda<Action&>(std::forward<Lambda>(lambda));
	Action action;
	action.player_idx = state.player_turn;
	for_each_empty_stone(state.board, [&](auto pos) {
		action.pos = pos;
		if (is_legal_move(state, action))
			return wrapped_lambda(action);
		return Continue;
	});
}

template <typename Lambda>
void for_each_liberty(
    const BoardState& state, const Group& group, Lambda&& lambda)
{
	auto wrapped_lambda = details::wrap_void_lambda<uint32_t>(
	    std::forward<Lambda>(lambda), DontExpand);
	uint32_t count = 0;

	Stone group_color = state.stones[group.parent];
	for_each_stone(state, group.parent, [&](uint32_t idx) {
		if (count == group.num_libs)
			return Break;
		if (state.stones[idx] == group_color)
		{
			return Expand;
		}
		else if (is_empty(state, idx))
		{
			count++;
			return wrapped_lambda(idx);
		}
		return DontExpand;
	});
}

template <typename Lambda>
void for_each_group(const GameState& game_state, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda<Group&>(std::forward<Lambda>(lambda));
	for (auto& group : game_state.group_table.groups)
	{
		if (group.size)
			if (wrapped_lambda(group) == Break)
				return;
	}
}

} // namespace go::engine

#endif // _ENGINE_ITERATORS_H_