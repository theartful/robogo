#include "mcts/playout.h"
#include "SimpleGUI/simplegui.h"
#include "engine/interface.h"
#include "engine/liberties.h"
#include "engine/utility.h"
#include "mcts/common.h"
#include "mcts/mcts.h"

#include <iostream>

using namespace go;
using namespace go::engine;

namespace go
{
namespace mcts
{
static constexpr bool EXTREME_DEBUGGING_MODE = false;

static inline void show_case(
    const char* case_title, const std::vector<Action>& actions_buffer,
    const GameState& game_state);

PlayoutPolicy::PlayoutPolicy()
{
	// seed prng
	std::array<typename PRNG::result_type, PRNG::state_size> random_data;
	std::random_device source;
	std::generate(
	    std::begin(random_data), std::end(random_data), std::ref(source));
	std::seed_seq seeds(std::begin(random_data), std::end(random_data));
	prng = PRNG(seeds);
}

void PlayoutPolicy::run_playout(GameState& game_state)
{
	if (!game_state.move_history.empty())
		last_move = game_state.move_history.back().pos;
	else
		last_move = Action::INVALID_ACTION;
	while (!is_terminal_state(game_state))
	{
		auto action = generate_move(game_state);
		if (is_invalid(action))
			break;
		if (!make_move(game_state, action))
		{
			std::cout << "run_playout: INVALID MOVE!!";
		}
		last_move = action.pos;
	}
}

Action PlayoutPolicy::generate_move(GameState& game_state)
{
	Action action{Action::INVALID_ACTION, game_state.player_turn};
	if (last_move < Action::PASS)
	{
		if (nearest_atari_capture(game_state))
			action = choose_action();
		else if (nearest_atari_defense(game_state))
			action = choose_action();
		else if (generate_low_lib(game_state))
			action = choose_action();
	}
	if (is_invalid(action))
	{
		if (general_atari_capture(game_state))
			action = choose_action();
		if (generate_all_moves(game_state))
			action = choose_action();
		else if (last_move != Action::PASS)
			action.pos = Action::PASS;
	}
	return action;
}

bool PlayoutPolicy::nearest_atari_capture(const GameState& game_state)
{
	auto& cluster = get_cluster(game_state.cluster_table, last_move);
	if (in_atari(cluster))
	{
		uint32_t atari_lib = get_atari_lib(cluster);
		add_action(atari_lib, game_state);
	}
	show_case("NEAREST ATARI CAPTURE", actions_buffer, game_state);
	return !actions_buffer.empty();
}

bool PlayoutPolicy::general_atari_capture(const GameState& game_state)
{
	auto& table = game_state.cluster_table;
	if (table.num_in_atari == 0)
		return false;
	uint32_t cur_atari_num = 0;
	for_each_cluster(game_state, [&](auto& cluster) {
		if (cluster.player == game_state.player_turn)
			return CONTINUE;
		if (!in_atari(cluster))
			return CONTINUE;
		cur_atari_num++;
		add_action(get_atari_lib(cluster), game_state);
		if (cur_atari_num == table.num_in_atari)
			return BREAK;
		return CONTINUE;
	});
	show_case("GENERAL ATARI CAPTURE", actions_buffer, game_state);

	return !actions_buffer.empty();
}

bool PlayoutPolicy::nearest_atari_defense(const GameState& game_state)
{
	using namespace go::engine;
	auto& table = game_state.cluster_table;
	auto& board = game_state.board_state;
	for_each_neighbor_cluster(table, board, last_move, [&](auto& cluster) {
		if (cluster.player != game_state.player_turn || !in_atari(cluster))
			return CONTINUE;
		uint32_t atari_lib = get_atari_lib(cluster);

		uint32_t num_empty = 0;
		for_each_neighbor(board, atari_lib, [&](uint32_t neighbor) {
			if (is_empty_cell(board, neighbor))
				num_empty++;
		});
		if (num_empty > 1)
		{
			add_action(atari_lib, game_state);
			return CONTINUE;
		}

		uint32_t new_libs = 0;
		for_each_neighbor_cluster(table, board, atari_lib, [&](auto& c) {
			// if friend cluster, add its liberties
			if (c.player == game_state.player_turn)
			{
				// not accurate because of shared liberties
				// but good enough!
				new_libs += c.num_liberties - 1;
				if (new_libs > 1)
				{
					add_action(atari_lib, game_state);
					return BREAK;
				}
			}
			// if enemy cluster and in atari, capture it
			else if (in_atari(c))
			{
				add_action(atari_lib, game_state);
				return BREAK;
			}
			return CONTINUE;
		});
		return CONTINUE;
	});
	show_case("NEAREST ATARI DEFENSE", actions_buffer, game_state);

	return !actions_buffer.empty();
}

bool PlayoutPolicy::generate_low_lib(const GameState& state)
{
	using namespace go::engine;

	auto& board = state.board_state;
	auto& table = state.cluster_table;
	auto& last_cluster = get_cluster(table, last_move);
	if (last_cluster.num_liberties == 2)
	{
		play_good_libs(last_cluster, state);
	}

	for_each_neighbor_cluster(table, board, last_move, [&](auto& c) {
		if (c.player == state.player_turn && c.num_liberties == 2)
		{
			play_good_libs(c, state);
		}
	});
	show_case("LOW LIB", actions_buffer, state);

	return !actions_buffer.empty();
}

bool PlayoutPolicy::generate_all_moves(const GameState& game_state)
{
	for (uint32_t pos = 0; pos < BoardState::MAX_NUM_CELLS; pos++)
	{
		if (is_empty_cell(game_state.board_state, pos))
			add_action(pos, game_state);
	}
	return !actions_buffer.empty();
}

void PlayoutPolicy::play_good_libs(
    const Cluster& cluster, const GameState& state)
{
	for_each_liberty(cluster, [&](uint32_t lib) {
		if (!is_self_atari(state, lib) && gains_liberties(lib, cluster, state))
			add_action(lib, state);
	});
}

bool PlayoutPolicy::gains_liberties(
    uint32_t lib, const Cluster& cluster, const GameState& state)
{
	auto& board = state.board_state;
	auto& table = state.cluster_table;
	uint32_t gain = 0;
	for_each_neighbor(board, lib, [&](uint32_t neighbor) {
		auto cell = board.board[neighbor];
		if (is_empty_cell(cell) && is_cluster_lib(cluster, neighbor))
		{
			if (++gain >= 2)
				return BREAK;
		}
		else if (cell == PLAYERS[state.player_turn])
		{
			auto& c = get_cluster(table, neighbor);
			if (c.parent_idx == cluster.parent_idx)
				return CONTINUE;
			for_each_liberty(c, [&](uint32_t c_lib) {
				if (!is_cluster_lib(cluster, c_lib))
					if (++gain >= 2)
						return BREAK;
				return CONTINUE;
			});
		}
		if (gain >= 2)
			return BREAK;
		return CONTINUE;
	});
	return gain >= 2;
}

void PlayoutPolicy::add_action(uint32_t pos, const GameState& state)
{
	Action action{pos, state.player_turn};
	if (is_acceptable(action, state))
		actions_buffer.push_back(action);
}

bool PlayoutPolicy::is_acceptable(const Action& action, const GameState& state)
{
	auto& board = state.board_state;
	auto& table = state.cluster_table;
	if (is_ko(board, action))
		return false;
	else if (is_surrounded(board, action.pos))
		return false;
	else if (is_suicide_move(table, board, action))
		return false;
	return true;
}

Action PlayoutPolicy::choose_action()
{
	using dist_range = decltype(dist)::param_type;
	auto random_idx = dist(prng, dist_range{0, actions_buffer.size() - 1});
	auto random_action = actions_buffer[random_idx];
	actions_buffer.clear();
	return random_action;
}

static inline void show_case(
    const char* case_title, const std::vector<Action>& actions_buffer,
    const GameState& game_state)
{
	if constexpr (EXTREME_DEBUGGING_MODE)
	{
		if (!actions_buffer.empty())
		{
			simplegui::BoardSimpleGUI::print_board(
			    game_state.board_state, game_state.player_turn);
			for (auto& action : actions_buffer)
			{
				std::cout
				    << simplegui::BoardSimpleGUI::get_alphanumeric_position(
				           action.pos)
				    << '\n';
			}
			std::cout << "TURN: "
			          << simplegui::BoardSimpleGUI::get_board_symbol(
			                 PLAYERS[game_state.player_turn], 0, 0)
			          << '\n';
			std::cout << case_title << '\n';
			std::string x;
			std::cin >> x;
		}
	}
}

} // namespace mcts
} // namespace go