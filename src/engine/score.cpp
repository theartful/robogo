#include "score.h"
#include "iterators.h"

#include <tuple>

namespace go::engine
{

static std::tuple<uint32_t, bool, bool> territory_points(
	const BoardState& state, uint32_t root, details::SearchCache& cache)
{
	uint32_t score = 1;
	cache.push(root);
	cache.mark_visited(root);
	std::array<bool, 3> player_stone = {false, false, false};
	while (!cache.empty())
	{
		uint32_t cur_pos = cache.pop();
		for_each_neighbor(state, cur_pos, [&](uint32_t neighbour) {
			if (is_empty(state, neighbour) && !cache.is_visited(neighbour))
			{
				cache.push(neighbour);
				cache.mark_visited(neighbour);
				score++;
			}
			else
			{
				Stone stone = state.stones[neighbour];
				player_stone[get_player_idx(stone)] = true;
			}
		});
	}
	return {score, player_stone[0], player_stone[1]};
}

std::pair<float, float>
calculate_score(const GameState& state, const Rules& rules)
{
	auto& board = state.board;
	auto& [black_player, white_player] = state.players;
	uint32_t white_territory_score = 0;
	uint32_t black_territory_score = 0;

	details::SearchCache cache;
	constexpr uint32_t board_begin = BoardState::EXTENDED_SIZE + 1;
	const uint32_t board_end = BoardState::index(board.size, board.size);
	for (uint32_t i = board_begin; i < board_end; i++)
	{
		if (!cache.is_visited(i) && is_empty(board, i))
		{
			auto [score, black_territory, white_territory] =
				territory_points(board, i, cache);

			if (black_territory && !white_territory)
				black_territory_score += score;
			else if (white_territory && !black_territory)
				white_territory_score += score;
		}
	}

	float white_score = white_territory_score + white_player.num_alive +
		white_player.num_captures + rules.komi;
	float black_score = black_territory_score + black_player.num_alive +
		black_player.num_captures;

	return {black_score, white_score};
}

}
