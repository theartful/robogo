#include "game.h"
#include "iterators.h"
#include "liberties.h"

namespace go::engine
{

static const std::array move_legality_strings = {
	"not empty", "ko", "suicide", "illegal action value", "legal"};

static std::tuple<uint32_t, bool, bool> territory_points(
	const BoardState& state, uint32_t root, details::SearchCache& cache);

MoveLegality
get_move_legality(const GameState& game, const Action& action)
{
	const auto& board = game.board;
	if (is_pass(action))
		return MoveLegality::Legal;
	else if (is_invalid(action))
		return MoveLegality::InvalidActionValue;
	else if (!is_empty(board, action.pos))
		return MoveLegality::IllegalNotEmpty;
	else if (is_simple_ko(game, action))
		return MoveLegality::IllegalKo;
	else if (is_suicide_move(game, action))
		return MoveLegality::IllegalSuicide;
	else
		return MoveLegality::Legal;
}

bool is_legal_move(const GameState& game, const Action& action)
{
	return get_move_legality(game, action) == MoveLegality::Legal;
}

bool is_simple_ko(const GameState& game, const Action& action)
{
	return game.player_turn == action.player_idx && game.board.ko == action.pos;
}

bool is_suicide_move(const GameState& game, const Action& action)
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

bool make_move(GameState& game, const Action& action)
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

void force_move(GameState& game, const Action& action)
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


bool is_terminal_state(const GameState& state)
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

}
