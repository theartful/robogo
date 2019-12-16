#ifndef SRC_AGENTS_MCTS_AGENT_H
#define SRC_AGENTS_MCTS_AGENT_H

#include "SimpleGUI/simplegui.h"
#include "controller/agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "mcts/mcts.h"
#include <algorithm>
#include <iostream>

namespace go
{

class MCTSAgent : public Agent
{
public:
	uint32_t generate_move(const Game& game) override
	{
		std::array<uint32_t, 4> corners = {
		    BoardState::index(3, 3), BoardState::index(15, 15),
		    BoardState::index(15, 3), BoardState::index(3, 15)};
		auto& game_state = game.get_game_state();
		if (game_state.move_history.size() <= 1)
		{
			auto& table = game_state.cluster_table;
			auto& board = game_state.board_state;
			for (auto corner : corners)
			{
				if (engine::is_valid_move(
				        table, board,
				        engine::Action{corner, game_state.player_turn}))
				{
					return corner;
				}
			}
		}
		if (will_win_if_pass(game_state))
			return Action::PASS;

		auto& history = game_state.move_history;
		if (history.size() >= 2)
			mcts_algo.advance_tree(*(history.rbegin() + 1), history.back());
		else
			mcts_algo.clear_tree();

		constexpr uint32_t DEFAULT_ALLOWED_TIME = 3000;
		auto& stats = mcts_algo.get_playout_stats();
		std::chrono::duration<uint32_t, std::milli> allowed_time{
		    DEFAULT_ALLOWED_TIME};
		if (!stats.moves_stats.empty())
		{
			auto& move_stat = stats.moves_stats.back();
			float average_playout_length =
			    float(move_stat.total_playout_length) /
			    float(move_stat.number_playouts + 0.01);
			int32_t average_num_moves =
			    std::ceil((average_playout_length + 0.1) / 2.0);
			int32_t remaining_time = static_cast<int32_t>(
			    game.get_remaining_time(get_player_idx()).count());
			if (remaining_time < 30000)
				return Action::PASS;

			allowed_time = std::chrono::duration<uint32_t, std::milli>{std::min(
			    std::max(
			        (remaining_time - 5 * 60000) / average_num_moves, 1000),
			    4500)};
		}
		std::cerr << "Allocated time for move: " << allowed_time.count()
		          << '\n';
		auto action = mcts_algo.run(game_state, allowed_time);
		show_debugging_info(game, action);
		return action.pos;
	}

	void show_debugging_info(const Game& game, const engine::Action& action)
	{
		/*
		auto player_turn = game.get_game_state().player_turn;
		auto state = game.get_game_state();

		engine::make_move(state, action);
		// simplegui::BoardSimpleGUI::print_board(state.board_state,
		// player_turn);
		std::cout << simplegui::BoardSimpleGUI::get_alphanumeric_position(
		                 action.pos)
		          << '\n';

		std::cout << "TURN: "
		          << simplegui::BoardSimpleGUI::get_board_symbol(
		                 engine::PLAYERS[player_turn], 0, 0)
		          << '\n';
		*/
		mcts_algo.show_debugging_info();
	}

	bool will_win_if_pass(const engine::GameState& state)
	{
		if (state.move_history.empty())
			return false;
		auto& last_action = state.move_history.back();
		if (engine::is_pass(last_action))
		{
			auto [black_score, white_score] = engine::calculate_score(state);
			float scores[2] = {black_score, white_score};
			return scores[state.player_turn] > scores[1 - state.player_turn];
		}
		return false;
	}

private:
	mcts::MCTS mcts_algo;
};

} // namespace go

#endif // SRC_AGENTS_MCTS_AGENT_H
