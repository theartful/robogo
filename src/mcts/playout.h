#ifndef SRC_MCTS_PLAYOUT_H
#define SRC_MCTS_PLAYOUT_H

#include "engine/board.h"
#include "mcts/common.h"

namespace go
{
namespace mcts
{

class PlayoutPolicy
{
public:
	PlayoutPolicy();
	void run_playout(engine::GameState& game_state);

private:
	engine::Action generate_move(engine::GameState& game_state);
	bool nearest_atari_capture(const engine::GameState& game_state);
	bool general_atari_capture(const engine::GameState& game_state);
	bool nearest_atari_defense(const engine::GameState& game_state);
	bool generate_low_lib(const engine::GameState& state);
	bool generate_all_moves(const engine::GameState& game_state);
	void play_good_libs(
	    const engine::Cluster& cluster, const engine::GameState& state);
	bool gains_liberties(
	    uint32_t lib, const engine::Cluster& cluster,
	    const engine::GameState& state);
	void add_action(uint32_t pos, const engine::GameState& state);
	bool
	is_acceptable(const engine::Action& action, const engine::GameState& state);
	engine::Action choose_action();

private:
	PRNG prng;
	std::uniform_int_distribution<size_t> dist;

	std::vector<engine::Action> actions_buffer;
	uint32_t last_move;
};

} // namespace mcts
} // namespace go

#endif // SRC_MCTS_PLAYOUT_H