#ifndef SRC_MCTS_MCTS_H
#define SRC_MCTS_MCTS_H

#include "engine/board.h"
#include "engine/cluster.h"
#include "mcts/common.h"
#include "mcts/playout.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
namespace go
{
namespace mcts
{

using NodeId = size_t;
using EdgeId = size_t;

constexpr NodeId INVALID_NODE_ID = std::numeric_limits<NodeId>::max();

struct Edge
{
	Edge(const engine::Action& action_, NodeId dest_)
	    : action{action_}, dest{dest_}
	{
	}
	engine::Action action;
	NodeId dest;
};

struct Node
{
	Node() : mcts_visits{0}, mcts_q{0}, rave_visits{0}, rave_q{0}
	{
	}
	uint32_t mcts_visits;
	float mcts_q;
	uint32_t rave_visits;
	float rave_q;
	std::vector<Edge> edges;

	void add_mcts_reward(float z)
	{
		mcts_visits++;
		mcts_q += (z - mcts_q) / mcts_visits;
	}
	void add_rave_reward(float z)
	{
		rave_visits++;
		rave_q += (z - rave_q) / rave_visits;
	}
};

struct Trajectory
{
	engine::GameState state;       // game state at the leaf node
	std::vector<NodeId> nodes_ids; // nodes along the in-tree trajectory;
	std::vector<EdgeId> edges_ids;
	uint32_t player_idx;
	uint32_t winner_idx;
	uint32_t playout_action_idx;
	uint32_t mcts_action_idx;

	std::array<std::array<bool, 2>, BoardState::MAX_NUM_CELLS> rave_actions;

	void visit(EdgeId edge_id, NodeId node_id)
	{
		nodes_ids.push_back(node_id);
		edges_ids.push_back(edge_id);
	}
	void reset(const engine::GameState& state_, NodeId root_id)
	{
		state = state_;
		nodes_ids.clear();
		edges_ids.clear();
		nodes_ids.push_back(root_id);
		std::fill(
		    rave_actions.begin(), rave_actions.end(), std::array{false, false});
		mcts_action_idx = state.move_history.size();
	};
	void start_playout()
	{
		playout_action_idx = state.move_history.size();
	}
};

struct MCTSStats
{
	size_t min_in_tree_depth;
	size_t max_in_tree_depth;
	float average_in_tree_depth;
	size_t play_count;
	size_t tot_depth;

	size_t num_simulations;
	std::array<size_t, 2> num_wins;

	MCTSStats()
	    : min_in_tree_depth{std::numeric_limits<uint32_t>::max()},
	      max_in_tree_depth{0}, average_in_tree_depth{0},
	      num_simulations{0}, num_wins{0, 0}
	{
	}

	void update(const Trajectory& traj)
	{
		auto depth = traj.nodes_ids.size();
		play_count++;
		min_in_tree_depth = std::min(min_in_tree_depth, depth);
		max_in_tree_depth = std::max(max_in_tree_depth, depth);
		tot_depth += depth;
		average_in_tree_depth = float(tot_depth) / play_count;
		num_simulations++;
		num_wins[traj.winner_idx]++;
	}
};

class MCTS
{
public:
	MCTS();
	engine::Action
	run(const engine::GameState&,
	    std::chrono::duration<uint32_t, std::milli> duration =
	        std::chrono::duration<uint32_t, std::milli>(0));

	const PlayoutStats& get_playout_stats();
	bool advance_tree(const engine::Action&, const engine::Action&);
	void clear_tree();
	void show_debugging_info();

private:
	NodeId allocate_node();
	NodeId allocate_root_node();
	EdgeId expand_node(NodeId, const engine::GameState&, const Trajectory&);
	bool expand_root_node(const engine::GameState&);
	Node& get_node(NodeId);
	Edge& get_edge(NodeId, EdgeId);
	bool is_expanded(const Node&);
	bool is_expanded(NodeId);
	float calculate_uct(const Node&, const Node&);
	float calculate_weighted_rave_value(const Node&);

	void update();
	void update_node_stats(const Trajectory&);
	void update_lgr(const Trajectory&);
	void update_rave(Trajectory&);

	// UCT
	EdgeId select_best_edge(const Node&);
	EdgeId select_best_edge(NodeId);

private:
	static constexpr size_t MAX_NODES_SIZE_IN_BYTES =
	    1UL * 1024UL * 1024UL * 1024UL; // 1 GB
	static constexpr size_t MAX_NUM_NODES =
	    MAX_NODES_SIZE_IN_BYTES / sizeof(Node);
	static constexpr size_t NUM_REUSE_LEVELS = 3;
	static constexpr uint32_t EXPANSION_THRESHOLD = 1;
	static constexpr float PASS_PROP = 0.01;

	static constexpr float RAVE_BIAS = 1.0 / 2003.0;

	PRNG prng;
	std::vector<Node> allocated_nodes;
	std::vector<Node> temporary_space;
	NodeId root_id;
	PlayoutPolicy playout_policy;
	MCTSStats stats;

	lgr::LGR lgr;
};

} // namespace mcts
} // namespace go

#endif // SRC_MCTS_MCTS_H