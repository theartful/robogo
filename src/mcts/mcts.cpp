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
			if (child.action.pos == actions[i].pos)
			{
				new_root_id = child.node_id;
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
				NodeId& child_id = child.node_id;
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

	stats = {};
	root_id = allocate_root_node();
	Trajectory traj;

	auto started = std::chrono::high_resolution_clock::now();
	for (uint32_t iteration = 0; iteration < MAX_ITERATIONS; iteration++)
	{
		traj.reset(root_state, root_id);
		auto& node_state = traj.state;

		auto node_id = root_id;

		// selection phase
		while (!is_terminal_state(node_state) && is_expanded(node_id))
		{
			const auto& action_child = select_best_child(node_id);
			make_move(node_state, action_child.action);
			node_id = action_child.node_id;
			traj.visit(node_id);
		}

		// expansion phase
		if (get_node(node_id).num_visits >= EXPANSION_THRESHOLD &&
		    !is_expanded(node_id) && !is_terminal_state(node_state))
		{
			const auto& action_child = expand_node(node_id, node_state);
			make_move(node_state, action_child.action);
			node_id = action_child.node_id;
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
		uint32_t his_win;
		if (playout_state.players[my_id].total_score >
		    playout_state.players[his_id].total_score)
			his_win = 0;
		else
			his_win = 1;

		for (NodeId traj_node_id : traj.nodes)
		{
			auto& node = get_node(traj_node_id);
			node.num_visits++;
			node.num_wins += his_win;
			his_win = 1 - his_win;
		}

		stats.update(traj);
	}
	auto done = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
	                 done - started)
	                 .count()
	          << '\n';

	Node& root_node = get_node(root_id);
	Action best_action = root_node.children[0].action;
	uint32_t max_visits = get_node(root_node.children[0].node_id).num_visits;
	for (auto& child : root_node.children)
	{
		auto& child_node = get_node(child.node_id);
		if (child_node.num_visits > max_visits)
		{
			max_visits = child_node.num_visits;
			best_action = child.action;
		}
	}
	return best_action;
}

Edge MCTS::expand_node(Node& node, const GameState& game_state)
{
	for_each_valid_action(game_state, [&](const Action& action) {
		if (!engine::will_be_surrounded(game_state, action.pos))
			node.children.emplace_back(action, allocate_node());
	});

	if (node.children.empty())
		node.children.emplace_back(
		    Action{Action::PASS, game_state.player_turn}, allocate_node());

	return node.children[0];
}

Edge MCTS::expand_node(NodeId node_id, const GameState& game_state)
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

float MCTS::calculate_uct(const Node& parent, const Node& child)
{
	static constexpr auto C = 0.7; // exploration constant

	float q_value = 0;
	if (child.num_visits == 0)
		q_value = 0;
	else
		q_value = static_cast<float>(child.num_wins) / child.num_visits;

	float exploration_term = sqrt(
	    log(static_cast<float>(parent.num_visits)) / (child.num_visits + 1));

	return q_value + C * exploration_term;
}

const Edge& MCTS::select_best_child(const Node& node)
{
	float max_uct_score =
	    calculate_uct(node, get_node(node.children.front().node_id));
	uint32_t child_index = 0;
	for (size_t i = 1; i < node.children.size(); i++)
	{
		Node& child = get_node(node.children[i].node_id);
		float uct_score = calculate_uct(node, child);
		if (uct_score > max_uct_score)
		{
			max_uct_score = uct_score;
			child_index = i;
		}
	}
	return node.children[child_index];
}

const Edge& MCTS::select_best_child(NodeId id)
{
	return select_best_child(get_node(id));
}

bool MCTS::is_expanded(const Node& node)
{
	return !node.children.empty();
}

bool MCTS::is_expanded(NodeId id)
{
	return is_expanded(get_node(id));
}

Node& MCTS::get_node(NodeId node_id)
{
	return allocated_nodes[node_id];
}

void MCTS::show_debugging_info()
{
	return;
	using namespace simplegui;
	if (allocated_nodes.empty() || allocated_nodes[root_id].children.empty())
		return;

	Node& root_node = allocated_nodes[0];
	auto most_visited = root_node.children[0];
	auto most_wins_to_visit = root_node.children[0];

	auto print_edge = [&](auto& edge) {
		auto pos = edge.action.pos;
		auto& child_node = get_node(edge.node_id);
		std::string pos_str = BoardSimpleGUI::get_alphanumeric_position(pos);
		printf(
		    "Action: %s num_visits: %d num_wins: %d\n", pos_str.c_str(),
		    child_node.num_visits, child_node.num_wins);
	};
	for (auto& child : root_node.children)
	{
		print_edge(child);
		auto& child_node = get_node(child.node_id);
		if (child_node.num_visits > get_node(most_visited.node_id).num_visits)
			most_visited = child;
		float v1 = 0;
		float v2 = 0;
		if (child_node.num_visits > 0)
			v1 = float(child_node.num_wins) / child_node.num_visits;
		if (get_node(most_wins_to_visit.node_id).num_visits > 0)
			v2 = float(get_node(most_wins_to_visit.node_id).num_wins) /
			     get_node(most_wins_to_visit.node_id).num_visits;
		if (v1 > v2)
			most_wins_to_visit = child;
	}
	printf("Most visited:\n");
	print_edge(most_visited);
	printf("Most wins to visit ratio:\n");
	print_edge(most_wins_to_visit);
	printf("Tree size: %lu\n", allocated_nodes.size());
	printf("Min in tree depth: %lu\n", stats.min_in_tree_depth);
	printf("Max in tree depth: %lu\n", stats.max_in_tree_depth);
	printf("Average in tree depth: %f\n", stats.average_in_tree_depth);
}

} // namespace mcts
} // namespace go
