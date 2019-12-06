#include "mcts/mcts.h"
#include "SimpleGUI/simplegui.h"
#include "engine/liberties.h"
#include "engine/utility.h"

#include <algorithm>
#include <iostream>
#include <iterator>

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

MCTS::MCTS() : root_id{INVALID_NODE_ID}
{
	allocated_nodes.reserve(MAX_NUM_NODES);
	temporary_space.reserve(
	    std::pow(BoardState::MAX_BOARD_SIZE + 1, 2 * NUM_REUSE_LEVELS));

	std::array<typename PRNG::result_type, PRNG::state_size> random_data;
	std::random_device source;
	std::generate(
	    std::begin(random_data), std::end(random_data), std::ref(source));
	std::seed_seq seeds(std::begin(random_data), std::end(random_data));
	prng = PRNG(seeds);
}

bool MCTS::advance_tree(const Action& action1, const Action& action2)
{
	if (root_id >= allocated_nodes.size())
		return false;

	NodeId new_root_id = root_id;
	Action actions[] = {action1, action2};
	for (auto i = 0; i < 2; i++)
	{
		bool found_new_root = false;
		for (auto& child : get_node(new_root_id).children)
		{
			if (child.first.pos == actions[i].pos)
			{
				new_root_id = child.second;
				found_new_root = true;
				break;
			}
		}
		if (!found_new_root)
		{
			clear_tree();
			return false;
		}
	}

	temporary_space.clear();
	temporary_space.push_back(get_node(new_root_id));
	root_id = 0;

	NodeId first = 0;
	NodeId last = 1;
	for (size_t level = 0; level < NUM_REUSE_LEVELS; level++)
	{
		for (NodeId it = first; it != last; it++)
		{
			for (auto& child : temporary_space[it].children)
			{
				NodeId& child_id = child.second;
				if (child_id == INVALID_NODE_ID)
					continue;
				temporary_space.push_back(std::move(get_node(child_id)));
				child_id = temporary_space.size() - 1;
			}
		}
		first = last;
		last = temporary_space.size();
		printf("first: %lu, last: %lu\n", first, last);
	}
	for (NodeId it = first; it != last; it++)
	{
		temporary_space[it].children.clear();
		temporary_space[it].child_to_expand =
		    std::numeric_limits<uint32_t>::max();
	}

	clear_tree();
	std::move(
	    temporary_space.begin(), temporary_space.end(),
	    std::back_inserter(allocated_nodes));
	return true;
}

void MCTS::clear_tree()
{
	allocated_nodes.clear();
}

Action MCTS::run(const GameState& root_state)
{
	constexpr uint32_t MAX_ITERATIONS = 50000;

	root_id = allocate_root_node();
	Trajectory traj;
	for (uint32_t iteration = 0; iteration < MAX_ITERATIONS; iteration++)
	{
		traj.reset(root_state, root_id);
		auto& node_state = traj.state;

		auto node_id = root_id;

		// selection phase
		while (!is_terminal_state(node_state) && is_fully_expanded(node_id))
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

		auto& playout_state = node_state;
		playout_policy.run_playout(playout_state);

		// calculate score
		calculate_score(
		    playout_state.board_state, playout_state.players[0],
		    playout_state.players[1]);

		// back propagation
		const uint32_t my_id = root_state.player_turn;
		const uint32_t his_id = 1 - my_id;
		int32_t his_win;
		if (playout_state.players[my_id].total_score >
		    playout_state.players[his_id].total_score)
			his_win = 0;
		else
			his_win = 1;

		for (NodeId traj_node_id : traj.nodes)
		{
			auto& node = get_node(traj_node_id);
			node.num_visits++;
			node.num_wins += static_cast<uint32_t>(his_win);
			his_win = 1 - his_win;
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
		return root_id;
}

void MCTS::fill_node_children(Node& node, const GameState& game_state)
{
	for_each_valid_action(game_state, [&](const Action& action) {
		if (!engine::is_surrounded(game_state.board_state, action.pos))
			node.children.emplace_back(action, INVALID_NODE_ID);
	});

	if (node.children.empty())
		node.children.emplace_back(
		    Action{Action::PASS, game_state.player_turn}, INVALID_NODE_ID);

	std::shuffle(node.children.begin(), node.children.end(), prng);
	node.child_to_expand = 0;
}

ActionChildPair MCTS::select_best_child(const Node& node)
{
	static constexpr auto C = 0.7; // exploration constant

	auto calc_utc = [&](const auto& action_node_pair) {
		const NodeId child_id = action_node_pair.second;
		Node& child = allocated_nodes[child_id];
		float exploitation_term =
		    static_cast<float>(child.num_wins) / child.num_visits;
		float exploration_term = sqrt(
		    log(static_cast<float>(node.num_visits)) / (child.num_visits + 1));

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

void MCTS::show_debugging_info()
{
	using namespace simplegui;
	Node& root_node = allocated_nodes[0];
	auto most_visited = root_node.children[0];
	auto most_wins_to_visit = root_node.children[0];

	auto print_child = [&](auto& child) {
		auto pos = child.first.pos;
		auto& child_node = get_node(child.second);
		std::string pos_str = BoardSimpleGUI::get_alphanumeric_position(pos);
		printf(
		    "Action: %s num_visits: %d num_wins: %d\n", pos_str.c_str(),
		    child_node.num_visits, child_node.num_wins);
	};
	for (auto& child : root_node.children)
	{
		//		print_child(child);
		auto& child_node = get_node(child.second);
		if (child_node.num_visits > get_node(most_visited.second).num_visits)
			most_visited = child;
		float v1 = float(child_node.num_wins) / child_node.num_visits;
		float v2 = float(get_node(most_wins_to_visit.second).num_wins) /
		           get_node(most_wins_to_visit.second).num_visits;
		if (v1 > v2)
			most_wins_to_visit = child;
	}
	printf("Most visited:\n");
	print_child(most_visited);
	printf("Most wins to visit ratio:\n");
	print_child(most_wins_to_visit);
}

} // namespace mcts
} // namespace go
