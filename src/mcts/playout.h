#ifndef SRC_MCTS_PLAYOUT_H
#define SRC_MCTS_PLAYOUT_H

#include "engine/board.h"
#include "mcts/common.h"

namespace go
{
namespace mcts
{

enum class PlayoutHeuristicType
{
	NEAREST_ATARI_CAPTURE,
	GENERAL_ATARI_CAPTURE,
	NEAREST_ATARI_DEFENSE,
	LOW_LIB
};

class PlayoutPolicy
{
public:
	PlayoutPolicy();
	void run_playout(engine::GameState& game_state);
	engine::Action generate_move(const engine::GameState& game_state);

private:
	bool nearest_atari_capture(const engine::GameState& game_state);
	bool general_atari_capture(const engine::GameState& game_state);
	bool nearest_atari_defense(const engine::GameState& game_state);
	bool generate_low_lib(const engine::GameState& state);
	void play_good_libs(
	    const engine::Cluster& cluster, const engine::GameState& state);
	bool gains_liberties(
	    uint32_t lib, const engine::Cluster& cluster,
	    const engine::GameState& state);
	void add_action(uint32_t pos, const engine::GameState& state);
	bool
	is_acceptable(const engine::Action& action, const engine::GameState& state);
	engine::Action choose_action(const engine::GameState& game_state);
	engine::Action generate_random_action(const engine::GameState& game_state);
	// not accurate. does not handle enemy captures well
	bool is_self_atari(const engine::GameState& game_state, uint32_t pos);

private:
	PRNG prng;
	std::uniform_int_distribution<size_t> dist;

	std::vector<engine::Action> actions_buffer;
	uint32_t last_move;
};

} // namespace mcts
} // namespace go

#endif // SRC_MCTS_PLAYOUT_H