#ifndef SRC_CONTROLLER_INTERFACE_H
#define SRC_CONTROLLER_INTERFACE_H

#include <array>
#include <memory>

#include "controller/agent.h"
#include "engine/board.h"
#include "engine/cluster.h"

namespace go
{

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
	void set_elapsed_time(
	    std::chrono::duration<uint32_t, std::milli> allowed_time,
	    uint32_t player_idx);

	const engine::GameState& get_game_state() const;
	const engine::BoardState& get_board_state() const;
	const engine::ClusterTable& get_cluster_table() const;

private:
	engine::GameState game_state;
	engine::ClusterTable cluster_table;
	std::array<std::shared_ptr<Agent>, 2> agents;
};

} // namespace go

#endif // SRC_CONTROLLER_INTERFACE_H
