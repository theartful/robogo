#include "controller/game.h"
#include "config.h"
#include "engine/interface.h"
#include "SimpleGUI/simplegui.h"
#include <iostream>

using namespace go;
using namespace go::engine;

Game::Game()
{
	set_game_end(false);
}

void Game::set_make_move_callback(std::function<void(bool)> callback)
{
	if (callback != NULL)
		make_move_callback = callback;
}

bool Game::register_agent(std::shared_ptr<Agent> agent, uint32_t player_idx)
{
	if (player_idx > 1)
		return false;
	else if (agents[player_idx])
		return false;
	else if (agent == agents[1 - player_idx])
		return false;

	agent->set_player_idx(player_idx);
	agents[player_idx] = std::move(agent);
	return true;
}

bool Game::unregister_agent(uint32_t player_idx)
{
	if (player_idx > 1)
		return false;
	agents[player_idx] = nullptr;
	return true;
}

void Game::set_allowed_time(
    std::chrono::duration<uint32_t, std::milli> allowed_time,
    uint32_t player_idx)
{
	std::lock_guard<std::mutex> lock(agent_mutex);
	agents_time_info[player_idx].set_allowed_time(allowed_time);
}


void Game::set_remaining_time(std::chrono::duration<uint32_t, std::milli> remaining_time,
	uint32_t agent_id)
{
	// std::lock_guard<std::mutex> lock(agent_mutex);
	// auto allowed_time = agents_time_info[agent_id].get_allowed_time();
	// auto new_elapsed_time = allowed_time - remaining_time;
	// agents_time_info[agent_id].set_elapsed_time(new_elapsed_time);
}

void Game::set_elapsed_time(
    std::chrono::duration<uint32_t, std::milli> elapsed_time,
    uint32_t player_idx)
{
	std::lock_guard<std::mutex> lock(agent_mutex);
	agents_time_info[player_idx].set_elapsed_time(elapsed_time);
}

const engine::GameState& Game::get_game_state() const
{
	return game_state;
}

const engine::BoardState& Game::get_board_state() const
{
	return game_state.board_state;
}

const engine::ClusterTable& Game::get_cluster_table() const
{
	return game_state.cluster_table;
}

bool Game::make_move(const Action& action)
{
	return engine::make_move(game_state, action);
}

void Game::force_moves(const std::vector<Action>& actions)
{
	engine::force_moves(game_state, actions);
}

void Game::main_loop()
{
	DEBUG_PRINT("GOING INTO THE LOOP.\n");
	while (!is_game_finished())
	{
		DEBUG_PRINT("IN THE LOOP.\n");
		auto& agent = agents[game_state.player_turn];
		auto& agent_time = agents_time_info[game_state.player_turn];

		agent_time.start_counting();
		std::cout << "GAME: Agent " << agent->get_player_idx()
		          << " will play!\n";
		Action agent_action = {agent->generate_move(*this),
		                       game_state.player_turn};
		std::cout << "GAME: Agent " << agent_action.player_index << " played "
		          << agent_action.pos << "\n";
		agent_time.stop_counting();

		// accept the move only if played in time
		bool valid_move = true;
		if (!agent_time.is_overtime())
		{
			if (!engine::make_move(game_state, agent_action))
			{
				valid_move = false;
				DEBUG_PRINT("INVALID MOVE\n!");
			}
		}

		if (make_move_callback != NULL)
			make_move_callback(valid_move);
	}
	DEBUG_PRINT("OUT OF THE GAME MAIN LOOP.\n");

	simplegui::BoardSimpleGUI::print_board(game_state.board_state, game_state.player_turn);
	auto [black_score, white_score] = engine::calculate_score(game_state);
	std::cout << "Black alive stones: " << game_state.players[0].number_alive_stones << '\n';
	std::cout << "Black captured enemies: " << game_state.players[0].number_captured_enemies << '\n';
	std::cout << "White alive stones: " << game_state.players[1].number_alive_stones << '\n';
	std::cout << "White captured enemies: " << game_state.players[1].number_captured_enemies << '\n';
	std::cout << "Black score: " << black_score << '\t'
	          << "White score: " << white_score << '\n';
}
