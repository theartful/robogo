#ifndef SRC_MCTS_PLAYOUT_H
#define SRC_MCTS_PLAYOUT_H

#include "engine/board.h"
#include "mcts/common.h"
#include "mcts/lgr.h"

namespace go
{
namespace mcts
{

// TODO: move this to someplace else
template <bool condition, typename T, typename U>
static inline constexpr auto conditional_v(T&& if_true, U&& if_false)
{
	if constexpr (condition)
		return if_true;
	return if_false;
}

static constexpr bool DETAILED_PLAYOUT_STATS = false;

enum class PlayoutHeuristicType : size_t
{
	LGR,
	NEAREST_ATARI_CAPTURE,
	GENERAL_ATARI_CAPTURE,
	NEAREST_ATARI_DEFENSE,
	LOW_LIB,
	PATTERN3X3,
	RANDOM,
	NUM_HEURISTICS
};

struct PlayoutStats
{
	struct SingleMoveStats
	{
		static constexpr size_t HITS_SIZE =
		    conditional_v<DETAILED_PLAYOUT_STATS>(
		        static_cast<size_t>(PlayoutHeuristicType::NUM_HEURISTICS), 0);

		uint32_t number_playouts = 0;
		uint32_t total_playout_length = 0;
		std::vector<uint32_t> playout_length;
		std::array<uint32_t, HITS_SIZE> hits = {};
	};

	std::array<
	    uint32_t, static_cast<size_t>(PlayoutHeuristicType::NUM_HEURISTICS)>
	    tot_hits = {};
	std::vector<SingleMoveStats> moves_stats;

	void add_heuristic_hit(PlayoutHeuristicType type)
	{
		/*
		size_t type_idx = static_cast<size_t>(type);
		tot_hits[type_idx]++;
		if constexpr (DETAILED_PLAYOUT_STATS)
			moves_stats.back().hits[type_idx]++;
		*/
	}

	void new_move_stats()
	{
		moves_stats.push_back({});
	}

	void register_new_playout(uint32_t playout_length)
	{
		if (!moves_stats.empty())
		{
			auto& stats = moves_stats.back();
			stats.number_playouts++;
			stats.total_playout_length += playout_length;
			if constexpr (DETAILED_PLAYOUT_STATS)
			{
				stats.playout_length.push_back(playout_length);
			}
		}
	}
};

class PlayoutPolicy
{
public:
	PlayoutPolicy(const lgr::LGR& lgr_);
	void init_new_move();
	uint32_t run_playout(engine::GameState& game_state);
	engine::Action generate_move(engine::GameState& game_state);
	const std::vector<engine::Action>& get_playout_history();
	const PlayoutStats& get_playout_stats();

private:
	engine::Action apply_lgr(const engine::GameState& game_state);
	engine::Action apply_default_policy(engine::GameState& game_state);
	engine::Action general_atari_capture(const engine::GameState& game_state);
	bool nearest_atari_capture(const engine::GameState& game_state);
	bool nearest_atari_defense(const engine::GameState& game_state);
	bool generate_low_lib(const engine::GameState& state);
	bool generate_pattern_moves(const engine::GameState& state);
	void play_good_libs(
	    const engine::Cluster& cluster, const engine::GameState& state);
	bool gains_liberties(
	    uint32_t lib, const engine::Cluster& cluster,
	    const engine::GameState& state);
	void add_action(uint32_t pos, const engine::GameState& state);
	bool
	is_acceptable(const engine::Action& action, const engine::GameState& state);
	engine::Action choose_action(const engine::GameState& game_state);
	engine::Action generate_random_action(engine::GameState& game_state);
	// not accurate. does not handle enemy captures well
	bool is_self_atari(const engine::GameState& game_state, uint32_t pos);
	uint32_t mercy_rule_applies(const engine::GameState& game_state);
	void init_atari_clusters(const engine::GameState& game_state);
	void update_atari_clusters(const engine::GameState& game_state);

private:
	PRNG prng;
	std::uniform_int_distribution<size_t> dist;

	std::vector<engine::Action> actions_buffer;
	std::bitset<BoardState::MAX_NUM_CELLS> atari_map;
	std::array<std::vector<uint16_t>, 2> clusters_in_atari;
	uint32_t last_move;

	const lgr::LGR& lgr;
	PlayoutStats playout_stats;
};

} // namespace mcts
} // namespace go

#endif // SRC_MCTS_PLAYOUT_H