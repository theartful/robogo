#include "controller/game.h"
#include "config.h"
#include "engine/interface.h"

using namespace go;
using namespace go::engine;

Game::Game()
{
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
	agents_time_info[player_idx].set_allowed_time(allowed_time);
}

void Game::set_elapsed_time(
    std::chrono::duration<uint32_t, std::milli> elapsed_time,
    uint32_t player_idx)
{
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

void Game::main_loop()
{
	while (!is_game_finished())
	{
		auto& agent = agents[game_state.player_turn];
		auto& agent_time = agents_time_info[game_state.player_turn];

		agent_time.start_counting();
		Action agent_action = {agent->generate_move(*this),
		                       game_state.player_turn};
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
	engine::calculate_score(
	    game_state.board_state, game_state.players[0], game_state.players[1]);
}
