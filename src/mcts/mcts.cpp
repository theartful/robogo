#include <algorithm>

#include "engine/utility.h"
#include "mcts/mcts.h"

using namespace go;
using namespace go::engine;
using namespace go::mcts;

namespace go
{
namespace mcts
{

Node::Node()
    : num_visits{0}, num_wins{0}, child_to_expand{
                                      std::numeric_limits<uint32_t>::max()}
{
}

MCTS::MCTS()
{
	allocated_nodes.reserve(MAX_NUM_NODES);
}

void MCTS::clear_tree()
{
	allocated_nodes.clear();
}

Action MCTS::run(const GameState& root_state)
{
	constexpr uint32_t MAX_ITERATIONS = 10000;
	constexpr uint32_t MAX_SIMULATION_DEPTH = 100;

	NodeId root_id = allocate_root_node();
	Trajectory traj;
	for (uint32_t iteration = 0; iteration < MAX_ITERATIONS; iteration++)
	{
		traj.reset(root_state);
		auto& node_state = traj.state;

		auto node_id = root_id;

		// selection phase
		while (is_terminal_state(node_state) && is_fully_expanded(node_id))
		{
			const auto& action_child = select_best_child(node_id);
			make_move(node_state, action_child.first);
			node_id = action_child.second;
			traj.visit(node_id);
		}

		// expansion phase
		if (!is_fully_expanded(node_id) && !is_terminal_state(node_state))
		{
			const auto& action_child = expand_node(node_id, node_state);
			make_move(node_state, action_child.first);
			node_id = action_child.second;
			traj.visit(node_id);
		}

		// simulation phase
		std::uniform_int_distribution<size_t> dist;
		using dist_range = decltype(dist)::param_type;
		for (uint32_t depth = 0;
		     depth < MAX_SIMULATION_DEPTH && !is_terminal_state(node_state);
		     depth++)
		{
			auto valid_actions = get_valid_actions(node_state);
			auto random_idx =
			    dist(prng, dist_range(0, valid_actions.size() - 1));
			auto random_action = valid_actions[random_idx];
			make_move(node_state, random_action);
		}

		// calculate score
		calculate_score(
		    node_state.board_state, node_state.players[0],
		    node_state.players[1]);

		// back propagation
		const uint32_t my_id = root_state.player_turn;
		const uint32_t his_id = 1 - my_id;
		int32_t my_win;
		if (node_state.players[my_id].total_score >
		    node_state.players[his_id].total_score)
			my_win = 1;
		else
			my_win = -1;

		for (NodeId traj_node_id : traj.nodes)
		{
			auto& node = get_node(traj_node_id);
			node.num_visits++;
			node.num_wins += static_cast<uint32_t>(my_win);
			my_win = -my_win;
		}
	}
	Node& root_node = get_node(root_id);
	Action best_action = root_node.children[0].first;
	uint32_t max_visits = get_node(root_node.children[0].second).num_visits;
	for (auto& child : root_node.children)
	{
		auto& child_node = get_node(child.second);
		if (child_node.num_visits > max_visits)
		{
			max_visits = child_node.num_visits;
			best_action = child.first;
		}
	}

	return best_action;
}

ActionChildPair MCTS::expand_node(Node& node, const GameState& game_state)
{
	if (node.children.size() == 0)
		fill_node_children(node, game_state);

	NodeId new_node_id = allocate_node();
	node.children[node.child_to_expand].second = new_node_id;
	return node.children[node.child_to_expand++];
}

ActionChildPair MCTS::expand_node(NodeId node_id, const GameState& game_state)
{
	return expand_node(get_node(node_id), game_state);
}

NodeId MCTS::allocate_node()
{
	if (allocated_nodes.size() >= MAX_NUM_NODES)
	{
		DEBUG_PRINT("MCTS has reached maximum size!\n");
	}
	allocated_nodes.emplace_back();
	return allocated_nodes.size() - 1;
}

NodeId MCTS::allocate_root_node()
{
	if (allocated_nodes.empty())
		return allocate_node();
	else
		return 0;
}

void MCTS::fill_node_children(Node& node, const GameState& game_state)
{
	// the same as "get_valid_actions" but repeating the code might be better
	// than getting the actions in a vector than copying it to the vector of
	// pairs
	for_each_valid_action(game_state, [&](const Action& action) {
		node.children.emplace_back(action, INVALID_NODE_ID);
	});

	if (node.children.empty())
		node.children.emplace_back(
		    Action{Action::PASS, game_state.player_turn}, INVALID_NODE_ID);

	std::shuffle(node.children.begin(), node.children.end(), prng);
	node.child_to_expand = 0;
}

std::vector<Action> MCTS::get_valid_actions(const GameState& game_state)
{
	std::vector<Action> valid_actions;
	for_each_valid_action(game_state, [&](const Action& action) {
		valid_actions.push_back(action);
	});

	if (valid_actions.empty())
		valid_actions.emplace_back(
		    Action{Action::PASS, game_state.player_turn});

	return valid_actions;
}

ActionChildPair MCTS::select_best_child(const Node& node)
{
	static constexpr auto C = 1.41421356237; // exploration constant

	auto calc_utc = [&](const auto& action_node_pair) {
		const NodeId child_id = action_node_pair.second;
		Node& child = allocated_nodes[child_id];
		float exploitation_term =
		    static_cast<float>(child.num_wins) / child.num_visits;
		float exploration_term =
		    sqrt(log(static_cast<float>(node.num_visits)) / (child.num_visits));

		return std::make_pair(
		    exploitation_term + C * exploration_term, action_node_pair);
	};

	auto get_max = [](const auto& a, const auto& b) {
		return (a.first > b.first) ? a : b;
	};

	auto best_score_child_pair = std::transform_reduce(
	    node.children.begin() + 1, node.children.end(),
	    calc_utc(*node.children.begin()), get_max, calc_utc);

	return best_score_child_pair.second;
}

ActionChildPair MCTS::select_best_child(NodeId id)
{
	return select_best_child(get_node(id));
}

bool MCTS::is_fully_expanded(const Node& node)
{
	return node.child_to_expand == node.children.size();
}

bool MCTS::is_fully_expanded(NodeId id)
{
	return is_fully_expanded(get_node(id));
}

Node& MCTS::get_node(NodeId node_id)
{
	return allocated_nodes[node_id];
}

} // namespace mcts
} // namespace go