#ifndef SRC_CONTROLLER_INTERFACE_H
#define SRC_CONTROLLER_INTERFACE_H

#include <array>
#include <chrono>
#include <memory>

#include "controller/agent.h"
#include "engine/board.h"
#include "engine/cluster.h"
#include "engine/interface.h"

#include <atomic>
#include <mutex>

namespace go
{

class AgentTime
{
public:
	AgentTime() : allowed_time{DEFAULT_ALLOWED_TIME}, elapsed_time{0}
	{
	}
	auto get_allowed_time() const
	{
		return allowed_time;
	}
	auto get_elapsed_time() const
	{
		return elapsed_time;
	}
	void
	set_allowed_time(std::chrono::duration<uint32_t, std::milli> allowed_time_)
	{
		this->allowed_time = allowed_time_;
	}
	void
	set_elapsed_time(std::chrono::duration<uint32_t, std::milli> elapsed_time_)
	{
		this->elapsed_time = elapsed_time_;
	}
	void start_counting()
	{
		move_start_time = std::chrono::steady_clock::now();
	}
	void stop_counting()
	{
		elapsed_time += std::chrono::duration_cast<std::chrono::milliseconds>(
		    std::chrono::steady_clock::now() - move_start_time);
	}
	bool is_overtime() const
	{
		return elapsed_time > allowed_time;
	}
	void reset()
	{
		elapsed_time = std::chrono::duration<uint32_t, std::milli>::zero();
	}

private:
	static constexpr uint32_t DEFAULT_ALLOWED_TIME = 15 * 60 * 1000;
	// maximum time allowed for a player throughout the game
	std::chrono::duration<uint32_t, std::milli> allowed_time;
	// the time from which it's this player's move
	// should be updated each time it's his turn
	std::chrono::steady_clock::time_point move_start_time;
	// should be updated when the player finishes his move
	// elapsed_time += duration(now - move_start_time);
	std::chrono::duration<uint32_t, std::milli> elapsed_time;
};

class Game
{
public:
	Game();
	void main_loop();
	bool make_move(const engine::Action& action);
	bool register_agent(std::shared_ptr<Agent> agent, uint32_t player_idx);
	bool unregister_agent(uint32_t player_idx);
	void set_allowed_time(
	    std::chrono::duration<uint32_t, std::milli> allowed_time,
	    uint32_t player_idx);
	auto get_allowed_time(uint32_t player_idx) const
	{
		return agents_time_info[player_idx].get_allowed_time();
	}
	auto get_elapsed_time(uint32_t player_idx) const
	{
		return agents_time_info[player_idx].get_elapsed_time();
	}
	void set_elapsed_time(
	    std::chrono::duration<uint32_t, std::milli> allowed_time,
	    uint32_t player_idx);
	bool is_game_finished()
	{
		if (get_game_end())
		{
			DEBUG_PRINT("THIS IS WHY IT ENDS.\n");
			return true;
		}
		else if (agents_time_info[0].is_overtime())
			return true;
		else if (agents_time_info[1].is_overtime())
			return true;
		else if (engine::is_terminal_state(game_state))
			return true;
		else
			return false;
	}

	bool get_game_end()
	{
		std::lock_guard<std::mutex> lock(game_mutex);
		return force_game_end;
	}

	void set_game_end(bool game_end)
	{
		DEBUG_PRINT("end_game() called.\n");
		std::lock_guard<std::mutex> lock(game_mutex);
		force_game_end = game_end;
	}

	const engine::GameState& get_game_state() const;
	const engine::BoardState& get_board_state() const;
	const engine::ClusterTable& get_cluster_table() const;

	void force_moves(const std::vector<engine::Action>& actions);

private:
	engine::GameState game_state;
	std::array<std::shared_ptr<Agent>, 2> agents;
	std::array<AgentTime, 2> agents_time_info;
	std::mutex game_mutex;
	bool force_game_end;
};

} // namespace go

#endif // SRC_CONTROLLER_INTERFACE_H
