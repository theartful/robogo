#include "engine.h"
#include "group.h"
#include "iterators.h"
#include "liberties.h"
#include <cmath>

using namespace go::engine;

static const std::array move_legality_strings = {
    "not empty", "ko", "suicide", "inlegal action value", "legal"};

static inline std::tuple<uint32_t, bool, bool> territory_points(
    const BoardState& state, uint32_t root, details::SearchCache& cache);

MoveLegality
go::engine::get_move_legality(const GameState& game, const Action& action)
{
	const auto& board = game.board;
	if (is_pass(action))
		return MoveLegality::Legal;
	else if (is_invalid(action))
		return MoveLegality::InvalidActionValue;
	else if (!is_empty(board, action.pos))
		return MoveLegality::IllegalNotEmpty;
	else if (is_simple_ko(board, action))
		return MoveLegality::IllegalKo;
	else if (is_suicide_move(game, action))
		return MoveLegality::IllegalSuicide;
	else
		return MoveLegality::Legal;
}

bool go::engine::is_legal_move(const GameState& game, const Action& action)
{
	return get_move_legality(game, action) == MoveLegality::Legal;
}

#include <iostream>
bool go::engine::is_suicide_move(const GameState& game, const Action& action)
{
	auto& board = game.board;
	auto& table = game.group_table;
	// move is not suicide if:
	//     1. a neighbor stone is empty, or
	//     2. a neighbor friend group has more than one liberty, or
	//     3. a neighbor enemy group will be captured
	if (get_empty_neighbors_count(board, action.pos))
		return false;

	bool is_suicide = true;
	for_each_neighbor(board, action.pos, [&](uint32_t neighbor) {
		if (board.stones[neighbor] == get_player_stone(action.player_idx))
		{
			if (!in_atari(get_group(table, neighbor)))
			{
				is_suicide = false;
				return Break;
			}
		}
		// enemy neighbor stone
		else
		{
			// if an enemy group will be captured
			if (in_atari(get_group(table, neighbor)))
			{
				is_suicide = false;
				return Break;
			}
		}
		return Continue;
	});
	return is_suicide;
}

static uint32_t get_ko(
    const GroupTable& table, const BoardState& state, uint32_t action_pos,
    uint32_t num_captured_stones)
{
	// there is a ko if:
	//     1. the previous move captured exactly one stone,
	//     2. the played stone has exactly one liberty, and
	//     3. the played stone's group consists only of it
	if (num_captured_stones != 1)
		return BoardState::INVALID_INDEX;

	auto& action_group = get_group(table, action_pos);
	if (in_atari(action_group) && action_group.size == 1)
	{
		uint32_t captured_stone_idx = 0;
		for_each_neighbor(state, action_pos, [&](auto neighbor) {
			if (is_empty(state, neighbor))
			{
				captured_stone_idx = neighbor;
				return Break;
			}
			return Continue;
		});
		return captured_stone_idx;
	}
	return BoardState::INVALID_INDEX;
}

bool go::engine::make_move(GameState& game, const Action& action)
{
	if (is_legal_move(game, action))
	{
		force_move(game, action);
		return true;
	}
	else
	{
		DEBUG_PRINT(
		    "engine::make_move: invalid move, case: %s\n",
		    move_legality_strings[static_cast<size_t>(
		        get_move_legality(game, action))]);
		return false;
	}
}

void go::engine::force_move(GameState& game, const Action& action)
{
	auto& table = game.group_table;
	auto& board = game.board;

	if (!is_pass(action))
	{
		game.players[game.player_turn].num_alive++;
		board.stones[action.pos] = get_player_stone(action.player_idx);
		uint32_t num_captured_stones = update_groups(game, action);
		board.ko = get_ko(table, board, action.pos, num_captured_stones);
	}

	game.player_turn = 1 - game.player_turn;
	game.move_history.push_back(action);
}

std::pair<float, float>
go::engine::calculate_score(const GameState& state, const Rules& rules)
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

static inline std::tuple<uint32_t, bool, bool> territory_points(
    const BoardState& state, uint32_t root, details::SearchCache& cache)
{
	uint32_t score = 1;
	cache.push(root);
	cache.mark_visited(root);
	std::array<bool, 2> player_stone = {false, false};
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

bool go::engine::is_terminal_state(const GameState& state)
{
	if (state.move_history.size() > 1)
	{
		const Action& last = state.move_history.back();
		const Action& before_last = state.move_history.rbegin()[1];
		return is_pass(last) && is_pass(before_last);
	}
	else
	{
		return false;
	}
}
