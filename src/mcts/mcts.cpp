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

MCTS::MCTS() : root_id{INVALID_NODE_ID}, playout_policy(lgr)
{
	allocated_nodes.reserve(MAX_NUM_NODES);
	temporary_space.reserve(
	    std::pow(BoardState::MAX_BOARD_SIZE + 1, 2 * NUM_REUSE_LEVELS));

	std::array<PRNG::result_type, PRNG::state_size> random_data;
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
		for (auto& child : get_node(new_root_id).edges)
		{
			if (child.action.pos == actions[i].pos)
			{
				new_root_id = child.dest;
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
			for (auto& child : temporary_space[it].edges)
			{
				NodeId& child_id = child.dest;
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
		temporary_space[it].edges.clear();
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
	constexpr uint32_t MAX_ITERATIONS = 50e3;

	stats = {};
	root_id = allocate_root_node();
	Trajectory traj;
	traj.player_idx = root_state.player_turn;

	auto started = std::chrono::high_resolution_clock::now();
	for (uint32_t iteration = 0; iteration < MAX_ITERATIONS; iteration++)
	{
		traj.reset(root_state, root_id);
		auto& node_state = traj.state;

		auto node_id = root_id;

		// selection phase
		while (!is_terminal_state(node_state) && is_expanded(node_id))
		{
			EdgeId best_edge_id = select_best_edge(node_id);
			auto& best_edge = get_edge(node_id, best_edge_id);
			make_move(node_state, best_edge.action);
			node_id = best_edge.dest;
			traj.visit(best_edge_id, node_id);
		}

		// expansion phase
		if (get_node(node_id).num_visits >= EXPANSION_THRESHOLD &&
		    !is_expanded(node_id) && !is_terminal_state(node_state))
		{
			EdgeId edge_id = expand_node(node_id, node_state);
			auto& edge = get_edge(node_id, edge_id);
			make_move(node_state, edge.action);
			node_id = edge.dest;
			traj.visit(edge_id, node_id);
		}

		auto& playout_state = node_state;
		playout_policy.run_playout(playout_state);

		auto [black_score, white_score] = calculate_score(playout_state);
		traj.winner_idx = black_score > white_score ? 0 : 1;

		// back propagation
		update_node_stats(traj);
		update_lgr(traj);

		stats.update(traj);
	}
	auto done = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
	                 done - started)
	                 .count()
	          << '\n';

	Node& root_node = get_node(root_id);
	Action best_action = root_node.edges[0].action;
	uint32_t max_visits = get_node(root_node.edges[0].dest).num_visits;
	for (auto& child : root_node.edges)
	{
		auto& child_node = get_node(child.dest);
		if (child_node.num_visits > max_visits)
		{
			max_visits = child_node.num_visits;
			best_action = child.action;
		}
	}
	return best_action;
}

void MCTS::update_node_stats(const Trajectory& traj)
{
	const uint32_t my_id = traj.player_idx;
	const uint32_t his_id = 1 - my_id;
	uint32_t his_win = traj.winner_idx == my_id ? 0 : 1;
	uint32_t winner_idx = 0;

	for (NodeId traj_node_id : traj.nodes_ids)
	{
		auto& node = get_node(traj_node_id);
		node.num_visits++;
		node.num_wins += his_win;
		his_win = 1 - his_win;
	}
}

void MCTS::update_lgr(const Trajectory& traj)
{
	const auto& playout_history = playout_policy.get_playout_history();
	for (uint32_t i = 1; i < playout_history.size(); ++i)
	{
		if (playout_history[i].player_index == traj.winner_idx)
			lgr.set_lgr(playout_history[i - 1], playout_history[i]);
		else
			lgr.remove_lgr(playout_history[i]);
	}
}

EdgeId MCTS::expand_node(Node& node, const GameState& game_state)
{
	for_each_valid_action(game_state, [&](const Action& action) {
		if (!engine::will_be_surrounded(game_state, action.pos))
			node.edges.emplace_back(action, allocate_node());
	});

	if (node.edges.empty())
		node.edges.emplace_back(
		    Action{Action::PASS, game_state.player_turn}, allocate_node());

	return 0;
}

EdgeId MCTS::expand_node(NodeId node_id, const GameState& game_state)
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

EdgeId MCTS::select_best_edge(const Node& node)
{
	float max_uct_score =
	    calculate_uct(node, get_node(node.edges.front().dest));
	uint32_t child_index = 0;
	for (size_t i = 1; i < node.edges.size(); i++)
	{
		Node& child = get_node(node.edges[i].dest);
		float uct_score = calculate_uct(node, child);
		if (uct_score > max_uct_score)
		{
			max_uct_score = uct_score;
			child_index = i;
		}
	}
	return child_index;
}

EdgeId MCTS::select_best_edge(NodeId id)
{
	return select_best_edge(get_node(id));
}

bool MCTS::is_expanded(const Node& node)
{
	return !node.edges.empty();
}

bool MCTS::is_expanded(NodeId id)
{
	return is_expanded(get_node(id));
}

Node& MCTS::get_node(NodeId node_id)
{
	return allocated_nodes[node_id];
}

Edge& MCTS::get_edge(NodeId node_id, EdgeId edge_id)
{
	return get_node(node_id).edges[edge_id];
}

void MCTS::show_debugging_info()
{
	using namespace simplegui;
	if (allocated_nodes.empty() || allocated_nodes[root_id].edges.empty())
		return;

	Node& root_node = allocated_nodes[0];
	auto most_visited = root_node.edges[0];
	auto most_wins_to_visit = root_node.edges[0];

	auto print_edge = [&](auto& edge) {
		auto pos = edge.action.pos;
		auto& child_node = get_node(edge.dest);
		std::string pos_str = BoardSimpleGUI::get_alphanumeric_position(pos);
		printf(
		    "Action: %s num_visits: %d num_wins: %d\n", pos_str.c_str(),
		    child_node.num_visits, child_node.num_wins);
	};
	for (auto& child : root_node.edges)
	{
		// print_edge(child);
		auto& child_node = get_node(child.dest);
		if (child_node.num_visits > get_node(most_visited.dest).num_visits)
			most_visited = child;
		float v1 = 0;
		float v2 = 0;
		if (child_node.num_visits > 0)
			v1 = float(child_node.num_wins) / child_node.num_visits;
		if (get_node(most_wins_to_visit.dest).num_visits > 0)
			v2 = float(get_node(most_wins_to_visit.dest).num_wins) /
			     get_node(most_wins_to_visit.dest).num_visits;
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
