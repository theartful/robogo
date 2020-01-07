#include "cluster.h"
#include "interface.h"
#include "liberties.h"
#include "utility.h"
#include <cmath>

using namespace go::engine;

static inline uint32_t territory_points(
    const BoardState&, unsigned char&, uint32_t, details::SearchCache&);

bool go::engine::is_valid_move(
    const ClusterTable& table, const BoardState& board_state,
    const Action& action)
{
	if (is_pass(action))
		return true;
	else if (is_invalid(action))
		return false;
	else if (!is_empty_cell(board_state, action.pos))
		return false;
	else if (is_ko(board_state, action))
		return false;
	else if (is_suicide_move(table, board_state, action))
		return false;
	else
		return true;
}

bool go::engine::is_suicide_move(
    const ClusterTable& table, const BoardState& board_state,
    const Action& action)
{
	bool is_suicide = true;
	// move is not suicide if:
	//     1. a neighbor cell is empty, or
	//     2. a neighbor friend cluster has more than one liberty, or
	//     3. a neighbor enemy cluster will be captured
	for_each_neighbor(board_state, action.pos, [&](uint32_t neighbor) {
		if (is_empty_cell(board_state, neighbor))
		{
			is_suicide = false;
			return BREAK;
		}
		else if (board_state.board[neighbor] == PLAYERS[action.player_index])
		{
			if (get_cluster(table, neighbor).num_liberties > 1)
			{
				is_suicide = false;
				return BREAK;
			}
		}
		// enemy neighbor cell
		else
		{
			// if an enemy cluster will be captured
			if (get_cluster(table, neighbor).num_liberties == 1)
			{
				is_suicide = false;
				return BREAK;
			}
		}
		return CONTINUE;
	});
	return is_suicide;
}

static uint32_t
get_ko(const ClusterTable& table, const BoardState& state, uint32_t action_pos)
{
	if (count_liberties(table, action_pos) != 1)
		return BoardState::INVALID_INDEX;

	uint32_t num_captured_stones = 0;
	uint32_t captured_stone_idx;
	for_each_neighbor_cluster(table, state, action_pos, [&](auto& cluster) {
		if (cluster.num_liberties == 1)
		{
			num_captured_stones += cluster.size;
			captured_stone_idx = cluster.parent_idx;
		}
	});

	// there is a ko if:
	//     1. the previous move captured exactly one stone,
	//     2. the played stone has exactly one liberty, and
	//     3. the played stone's cluster consists only of it
	if (get_cluster(table, action_pos).size == 1 && num_captured_stones == 1)
		return captured_stone_idx;
	else
		return BoardState::INVALID_INDEX;
}

bool go::engine::make_move(GameState& game_state, const Action& action)
{
	ClusterTable& table = game_state.cluster_table;
	BoardState& board_state = game_state.board_state;
	if (is_valid_move(table, board_state, action))
	{
		if (!is_pass(action))
		{
			game_state.players[game_state.player_turn].number_alive_stones++;
			board_state.board[action.pos] = PLAYERS[action.player_index];
			board_state.ko = get_ko(table, board_state, action.pos);
			update_clusters(game_state, action);
		}

		game_state.number_played_moves++;
		game_state.player_turn = 1 - game_state.player_turn;
		game_state.move_history.push_back(action);

		return true;
	}
	else
	{
		DEBUG_PRINT("engine::make_move: invalid move!\n");
		return false;
	}
}

void go::engine::calculate_score(
    const BoardState& boardState, Player& black_player, Player& white_player)
{
	uint32_t white_territory_score = 0, black_territory_score = 0,
	         score_temp = 0;

	// To detect wether the traversed territory belong to which side, "01" for
	// white and "10" for black
	unsigned char territory_type; // if the output of the traversed territory
	                              // was "11" the it was a false territory
	details::SearchCache
	    search_cache; // to avoid starting traversing a new territory from an
	                  // already traversed empty cell

	// Traversing the board to detect any start of any territory
	for (uint32_t i = 0; i < BoardState::MAX_NUM_CELLS; i++)
	{
		if (!search_cache.is_visited(i) && is_empty_cell(boardState, i))
		{
			territory_type = 0;
			score_temp =
			    territory_points(boardState, territory_type, i, search_cache);
			// ANDing the territory_type to figure out the output of the
			// traversed territory
			if ((territory_type & static_cast<unsigned char>(Cell::BLACK)) == 0)
				white_territory_score += score_temp;
			else if (
			    (territory_type & static_cast<unsigned char>(Cell::WHITE)) == 0)
				black_territory_score += score_temp;
		}
	}

	// Updating scores
	white_player.total_score =
	    white_territory_score + white_player.number_alive_stones +
	    white_player.number_captured_enemies; //+ Rules::KOMI;
	black_player.total_score = black_territory_score +
	                           black_player.number_alive_stones +
	                           black_player.number_captured_enemies;
}

static inline uint32_t territory_points(
    const BoardState& state, unsigned char& territory_type, uint32_t root,
    details::SearchCache& search_cache)
{
	uint32_t score = 1;
	search_cache.push(root);
	search_cache.mark_visited(root);
	while (!search_cache.empty())
	{
		uint32_t cur_pos = search_cache.pop();
		for_each_neighbor(state, cur_pos, [&](uint32_t neighbour) {
			if (is_empty_cell(state, neighbour) &&
			    !search_cache.is_visited(neighbour))
			{
				search_cache.push(neighbour);
				search_cache.mark_visited(neighbour);
				score++;
			}
			else
			{
				territory_type |=
				    static_cast<unsigned char>(state.board[neighbour]);
			}
		});
	}
	return score;
}

bool go::engine::is_terminal_state(const GameState& state)
{
	if (state.move_history.size() > 1)
	{
		const Action& last = state.move_history.back();
		const Action& before_last = state.move_history.rbegin()[1];
		return is_pass(last) && is_pass(before_last);
	}
	else
	{
		return false;
	}
}
