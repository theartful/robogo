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
auto wrap_void_lambda(Lambda lambda)
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
void for_each_neighbor(uint32_t cell_idx, Lambda lambda)
{
	auto wrapped_lambda = details::wrap_void_lambda(lambda);
	// right
	if (cell_idx + 1 < BoardState::MAX_NUM_CELLS)
		if (wrapped_lambda(cell_idx + 1) == BREAK)
			return;
	// up
	if (cell_idx >= BoardState::MAX_BOARD_SIZE)
		if (wrapped_lambda(cell_idx - BoardState::MAX_BOARD_SIZE) == BREAK)
			return;
	// left
	if (cell_idx >= 1)
		if (wrapped_lambda(cell_idx - 1) == BREAK)
			return;
	// down
	if (cell_idx + BoardState::MAX_BOARD_SIZE < BoardState::MAX_NUM_CELLS)
		if (wrapped_lambda(cell_idx + BoardState::MAX_BOARD_SIZE) == BREAK)
			return;
}

template <typename Lambda, typename CVClusterTable>
void for_each_neighbor_cluster(
    CVClusterTable& table, const BoardState& state, uint32_t cell_idx,
    Lambda lambda)
{
	auto wrapped_lambda = details::wrap_void_lambda<Cluster&>(lambda);
	uint32_t visited[4];
	uint32_t visited_count = 0;
	for_each_neighbor(cell_idx, [&](uint32_t neighbor) {
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
		return (cache[index] & VISIT_BIT) == VISIT_BIT;
	}
	void mark_visited(uint32_t index)
	{
		cache[index] |= VISIT_BIT;
	}
	uint32_t cache[BoardState::MAX_NUM_CELLS] = {};
	int32_t top_index = -1;
};
} // namespace details

template <typename Lambda>
void for_each_cell(uint32_t root, Lambda lambda)
{
	auto wrapped_lambda = details::wrap_void_lambda<uint32_t, EXPAND>(lambda);
	details::SearchCache search_cache;
	search_cache.push(root);
	search_cache.mark_visited(root);

	while (!search_cache.empty())
	{
		uint32_t cur_pos = search_cache.pop();
		if (wrapped_lambda(cur_pos) == EXPAND)
		{
			for_each_neighbor(cur_pos, [&](uint32_t neighbour) {
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
    const Cluster& cluster, const BoardState& state, Lambda lambda)
{
	auto wrapped_lambda = details::wrap_void_lambda<uint32_t, EXPAND>(lambda);
	for_each_cell(cluster.parent_idx, [&](uint32_t idx) {
		if (state.board[idx] == PLAYERS[cluster.player])
			return wrapped_lambda(idx);
		else
			return DONT_EXPAND;
	});
}

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_UTILITY_H_