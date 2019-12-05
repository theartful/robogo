#ifndef TREE_SEARCH_NODE_H
#define TREE_SEARCH_NODE_H

#include "engine/board.h"
#include "engine/cluster.h"
#include "lgr.h"
#include <memory>
#include <random>

namespace go
{
namespace mcts
{

using NodeId = size_t;
using ActionChildPair = std::pair<engine::Action, NodeId>;
using PRNG = std::mt19937;

constexpr NodeId INVALID_NODE_ID = ~(0UL);

struct Node
{
	Node();
	uint32_t num_visits;
	uint32_t num_wins;
	std::vector<ActionChildPair> children;
	uint32_t child_to_expand;
};

struct Trajectory
{
	engine::GameState state;   // game state at the leaf node
	std::vector<NodeId> nodes; // nodes along the trajectory;

	void visit(NodeId id)
	{
		nodes.push_back(id);
	}

	void reset(const engine::GameState& state_)
	{
		state = state_;
		nodes.clear();
	};
};

class MCTS
{
public:
	MCTS();
	engine::Action run(const engine::GameState& root_state);
	void clear_tree();

private:
	NodeId allocate_node();
	NodeId allocate_root_node();
	ActionChildPair expand_node(Node&, const engine::GameState&);
	ActionChildPair expand_node(NodeId, const engine::GameState&);
	void fill_node_children(Node&, const engine::GameState&);
	Node& get_node(NodeId);
	bool is_fully_expanded(const Node&);
	bool is_fully_expanded(NodeId);
	std::vector<engine::Action> get_valid_actions(const engine::GameState&);

	// UCT
	ActionChildPair select_best_child(const Node& node);
	ActionChildPair select_best_child(NodeId node_id);

private:
	static constexpr size_t MAX_NODES_SIZE_IN_BYTES =
	    1UL * 1024UL * 1024UL * 1024UL; // 2 GB
	static constexpr size_t MAX_NUM_NODES =
	    MAX_NODES_SIZE_IN_BYTES / sizeof(Node);

	std::vector<Node> allocated_nodes;
	PRNG prng;

	lgr::LGR lgr;

};

} // namespace mcts
} // namespace go

#endif // TREE_SEARCH_NODE_H