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
		if (get_node(node_id).mcts_visits >= EXPANSION_THRESHOLD &&
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
		update_rave(traj);

		stats.update(traj);
	}
	auto done = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
	                 done - started)
	                 .count()
	          << '\n';

	Node& root_node = get_node(root_id);
	Action best_action = root_node.edges[0].action;
	uint32_t max_visits = get_node(root_node.edges[0].dest).mcts_visits;
	for (auto& child : root_node.edges)
	{
		auto& child_node = get_node(child.dest);
		if (child_node.mcts_visits > max_visits)
		{
			max_visits = child_node.mcts_visits;
			best_action = child.action;
		}
	}
	return best_action;
}

void MCTS::update_node_stats(const Trajectory& traj)
{
	const uint32_t my_id = traj.player_idx;
	const uint32_t his_id = 1 - my_id;
	float his_win = traj.winner_idx == my_id ? 0.0f : 1.0f;
	uint32_t winner_idx = 0;

	for (NodeId traj_node_id : traj.nodes_ids)
	{
		auto& node = get_node(traj_node_id);
		node.add_mcts_reward(his_win);
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

void MCTS::update_rave(Trajectory& traj)
{
	if (traj.nodes_ids.size() <= 2)
		return;

	auto mark_action = [&](const Action& a) {
		if (!is_invalid(a) && !is_pass(a))
			traj.rave_actions[a.pos][a.player_index] = true;
	};
	auto is_marked = [&](const Action& a) {
		if (!is_invalid(a) && !is_pass(a))
			return traj.rave_actions[a.pos][a.player_index];
		return false;
	};

	for (auto& action : playout_policy.get_playout_history())
		mark_action(action);

	for (size_t i = traj.nodes_ids.size() - 2; (i--) > 0;)
	{
		NodeId node_id = traj.nodes_ids[i];
		EdgeId played_edge_id = traj.edges_ids[i];
		Node& node = get_node(node_id);
		Edge& played_edge = get_edge(node_id, played_edge_id);
		mark_action(played_edge.action);
		for (Edge& edge : node.edges)
		{
			if (is_marked(edge.action))
			{
				float z =
				    edge.action.player_index == traj.winner_idx ? 1.0f : 0.0f;
				Node& next_node = get_node(edge.dest);
				next_node.add_rave_reward(z);
			}
		}
	}
}

EdgeId MCTS::expand_node(NodeId node_id, const GameState& game_state)
{
	Node& node = get_node(node_id);

	for_each_valid_action(game_state, [&](const Action& action) {
		if (!engine::will_be_surrounded(game_state, action.pos))
			node.edges.emplace_back(action, node_id, allocate_node());
	});

	if (node.edges.empty())
		node.edges.emplace_back(
		    Action{Action::PASS, game_state.player_turn}, node_id,
		    allocate_node());

	return 0;
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

	const float q_value = child.mcts_q;
	float exploration_term = sqrt(
	    log(static_cast<float>(parent.mcts_visits)) / (child.mcts_visits + 1));

	return q_value + C * exploration_term;
}

float MCTS::calculate_weighted_rave_value(const Node& child)
{
	float weight =
	    child.rave_visits / (child.rave_visits + child.mcts_visits +
	                         child.rave_visits * child.mcts_visits * RAVE_BIAS);
	return weight * child.rave_q + (1.0 - weight) * child.mcts_q;
}

EdgeId MCTS::select_best_edge(const Node& node)
{
	float max_q =
	    calculate_weighted_rave_value(get_node(node.edges.front().dest));
	uint32_t child_index = 0;
	for (size_t i = 1; i < node.edges.size(); i++)
	{
		Node& child = get_node(node.edges[i].dest);
		float q = calculate_weighted_rave_value(child);
		if (q > max_q)
		{
			max_q = q;
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
		    "Action: %s mcts_visits: %d mcts_q: %f, rave_visits: %d, "
		    "rave q: %f\n",
		    pos_str.c_str(), child_node.mcts_visits, child_node.mcts_q,
		    child_node.rave_visits, child_node.rave_q);
	};
	for (auto& child : root_node.edges)
	{
		// print_edge(child);
		auto& child_node = get_node(child.dest);
		if (child_node.mcts_visits > get_node(most_visited.dest).mcts_visits)
			most_visited = child;
	}
	printf("Most visited:\n");
	print_edge(most_visited);
	printf("Tree size: %lu\n", allocated_nodes.size());
	printf("Min in tree depth: %lu\n", stats.min_in_tree_depth);
	printf("Max in tree depth: %lu\n", stats.max_in_tree_depth);
	printf("Average in tree depth: %f\n", stats.average_in_tree_depth);
}

} // namespace mcts
} // namespace go
