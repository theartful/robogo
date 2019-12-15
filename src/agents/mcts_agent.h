#ifndef SRC_AGENTS_MCTS_AGENT_H
#define SRC_AGENTS_MCTS_AGENT_H

#include "SimpleGUI/simplegui.h"
#include "controller/agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "mcts/mcts.h"
#include <iostream>

namespace go
{

class MCTSAgent : public Agent
{
public:
	uint32_t generate_move(const Game& game) override
	{
		auto& game_state = game.get_game_state();
		if (will_win_if_pass(game_state))
			return Action::PASS;

		auto& history = game_state.move_history;
		if (history.size() >= 2)
			mcts_algo.advance_tree(*(history.rbegin() + 1), history.back());
		else
			mcts_algo.clear_tree();

		auto action = mcts_algo.run(game_state);
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
			auto [black_score, white_score] = 
				engine::calculate_score(state);
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
