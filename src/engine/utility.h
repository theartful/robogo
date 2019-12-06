#ifndef SRC_ENGINE_UTILITY_H_
#define SRC_ENGINE_UTILITY_H_

#include <algorithm>
#include <array>
#include <type_traits>

#include "board.h"
#include "cluster.h"

#include "interface.h"

namespace go
{
namespace engine
{

enum ForPolicy
{
	EXPAND,
	DONT_EXPAND,
	CONTINUE,
	BREAK,
};

namespace details
{
template <typename Arg = uint32_t, ForPolicy policy = CONTINUE, typename Lambda>
auto wrap_void_lambda(Lambda&& lambda)
{
	using namespace std;
	if constexpr (!is_same_v<decltype(lambda(declval<Arg>())), void>)
		return lambda;
	else
		return [lambda](auto&& val) {
			lambda(val);
			return policy;
		};
}
} // namespace details

template <typename Lambda>
void for_each_neighbor(
    const BoardState& state, uint32_t cell_idx, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda(std::forward<Lambda>(lambda));
	// right
	if (uint32_t right = cell_idx + 1; state.board[right] != Cell::BORDER)
		if (wrapped_lambda(right) == BREAK)
			return;
	// up
	if (uint32_t up = cell_idx - BoardState::EXTENDED_BOARD_SIZE;
	    state.board[up] != Cell::BORDER)
		if (wrapped_lambda(up) == BREAK)
			return;
	// left
	if (uint32_t left = cell_idx - 1; state.board[left] != Cell::BORDER)
		if (wrapped_lambda(left) == BREAK)
			return;
	// down
	if (uint32_t down = cell_idx + BoardState::EXTENDED_BOARD_SIZE;
	    state.board[down] != Cell::BORDER)
		if (wrapped_lambda(down) == BREAK)
			return;
}

template <typename Lambda, typename CVClusterTable>
void for_each_neighbor_cluster(
    CVClusterTable& table, const BoardState& state, uint32_t cell_idx,
    Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda<Cluster&>(std::forward<Lambda>(lambda));
	uint32_t visited[4];
	uint32_t visited_count = 0;
	for_each_neighbor(state, cell_idx, [&](uint32_t neighbor) {
		if (!is_empty_cell(state.board[neighbor]))
		{
			uint32_t cluster_idx = get_cluster_idx(table, neighbor);
			// check if visited
			const auto begin_it = visited;
			const auto end_it = visited + visited_count;
			if (std::find(begin_it, end_it, cluster_idx) == end_it)
			{
				visited[visited_count++] = cluster_idx;
				return wrapped_lambda(table.clusters[cluster_idx]);
			}
		}
		return CONTINUE;
	});
}

namespace details
{
struct SearchCache
{
	static constexpr uint32_t VISIT_BIT = 1U << 31;
	bool empty() const
	{
		return top_index == -1;
	}
	uint32_t pop()
	{
		assert(top_index >= 0);
		return cache[top_index--] & (~VISIT_BIT);
	}
	void push(uint32_t value)
	{
		top_index++;
		// clear all except VISIT_BIT
		cache[top_index] &= VISIT_BIT;
		// store the value in the first 31 bits;
		cache[top_index] |= value;
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
	uint32_t cache[BoardState::MAX_NUM_CELLS] = {};
	int32_t top_index = -1;
};
} // namespace details

template <typename Lambda>
void for_each_cell(const BoardState& state, uint32_t root, Lambda&& lambda)
{
	details::SearchCache search_cache;
	search_cache.push(root);
	search_cache.mark_visited(root);

	while (!search_cache.empty())
	{
		uint32_t cur_pos = search_cache.pop();
		if (lambda(cur_pos) == EXPAND)
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
void for_each_cluster_cell(
    const Cluster& cluster, const BoardState& state, Lambda&& lambda)
{
	auto wrapped_lambda = details::wrap_void_lambda<uint32_t, EXPAND>(
	    std::forward<Lambda>(lambda));
	Cell cluster_color = state.board[cluster.parent_idx];
	for_each_cell(state, cluster.parent_idx, [&](uint32_t idx) {
		if (state.board[idx] == cluster_color)
			return wrapped_lambda(idx);
		else
			return DONT_EXPAND;
	});
}

template <typename Lambda>
void for_each_empty_cell(const BoardState& state, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda<uint32_t>(std::forward<Lambda>(lambda));
	for (uint32_t i = 0; i < static_cast<uint32_t>(state.board.size()); i++)
	{
		if (state.board[i] == Cell::EMPTY)
			if (wrapped_lambda(i) == BREAK)
				return;
	}
}

// if there are no actions, calls lambda on pass, otherwise pass is not
// considered
template <typename Lambda>
void for_each_valid_action(const GameState& state, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda<Action&>(std::forward<Lambda>(lambda));
	Action action;
	action.player_index = state.player_turn;
	for_each_empty_cell(state.board_state, [&](auto pos) {
		action.pos = pos;
		if (is_valid_move(state.cluster_table, state.board_state, action))
			return wrapped_lambda(action);
		return CONTINUE;
	});
}

template <typename Lambda>
void for_each_liberty(const Cluster& cluster, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda(std::forward<Lambda>(lambda));
	uint32_t count = 0;
	for (uint32_t pos = 0;; pos++)
	{
		if (cluster.liberties_map[pos])
		{
			if (wrapped_lambda(pos) == BREAK)
				return;
			count++;
		}
		if (count == cluster.num_liberties)
			return;
	}
}

template <typename Lambda>
void for_each_cluster(const GameState& game_state, Lambda&& lambda)
{
	auto wrapped_lambda =
	    details::wrap_void_lambda<Cluster&>(std::forward<Lambda>(lambda));
	for (uint32_t i = 0; i < BoardState::MAX_NUM_CELLS; i++)
	{
		auto cluster = game_state.cluster_table.clusters[i];
		if (i != cluster.parent_idx || !cluster.size)
			continue;
		if (wrapped_lambda(cluster) == BREAK)
			return;
	}
}

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_UTILITY_H_