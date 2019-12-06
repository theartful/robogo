#ifndef SRC_MCTS_MCTS_H
#define SRC_MCTS_MCTS_H

#include "engine/board.h"
#include "engine/cluster.h"
#include "mcts/common.h"
#include "mcts/playout.h"
#include <memory>

namespace go
{
namespace mcts
{

using NodeId = size_t;
using ActionChildPair = std::pair<engine::Action, NodeId>;

constexpr NodeId INVALID_NODE_ID = std::numeric_limits<NodeId>::max();

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
	void reset(const engine::GameState& state_, NodeId root_id)
	{
		state = state_;
		nodes.clear();
		nodes.push_back(root_id);
	};
};

class MCTS
{
public:
	MCTS();
	engine::Action run(const engine::GameState&);
	bool advance_tree(const engine::Action&, const engine::Action&);
	void clear_tree();
	void show_debugging_info();

private:
	NodeId allocate_node();
	NodeId allocate_root_node();
	ActionChildPair expand_node(Node&, const engine::GameState&);
	ActionChildPair expand_node(NodeId, const engine::GameState&);
	void fill_node_children(Node&, const engine::GameState&);
	Node& get_node(NodeId);
	bool is_fully_expanded(const Node&);
	bool is_fully_expanded(NodeId);

	// UCT
	ActionChildPair select_best_child(const Node&);
	ActionChildPair select_best_child(NodeId);

private:
	static constexpr size_t MAX_NODES_SIZE_IN_BYTES =
	    1UL * 1024UL * 1024UL * 1024UL; // 2 GB
	static constexpr size_t MAX_NUM_NODES =
	    MAX_NODES_SIZE_IN_BYTES / sizeof(Node);
	static constexpr size_t NUM_REUSE_LEVELS = 2;

	PRNG prng;
	std::vector<Node> allocated_nodes;
	std::vector<Node> temporary_space;
	NodeId root_id;
	PlayoutPolicy playout_policy;
};

} // namespace mcts
} // namespace go

#endif // SRC_MCTS_MCTS_H