#include "cluster.h"
#include "interface.h"
#include "liberties.h"
#include "utility.h"
#include <cmath>

using namespace go::engine;

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
	else if (action.pos == board_state.ko)
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
	for_each_neighbor(action.pos, [&](uint32_t neighbor) {
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
	if (get_cluster(table, action_pos).size == 1 && num_captured_stones == 1 &&
	    count_liberties(table, action_pos) == 1)
		return captured_stone_idx;
	else
		return BoardState::INVALID_INDEX;
}

bool go::engine::make_move(
    ClusterTable& table, GameState& game_state, const Action& action)
{
	BoardState& board_state = game_state.board_state;
	if (is_valid_move(table, board_state, action))
	{
		// TODO: update player number_alive_stones and number_captured_enemies
		// will require update_clusters to return some information
		if (!is_pass(action))
		{
			board_state.board[action.pos] = PLAYERS[action.player_index];
			board_state.ko = get_ko(table, board_state, action.pos);
			update_clusters(table, board_state, action);
		}

		game_state.number_played_moves++;
		game_state.player_turn = 1 - game_state.player_turn;
		game_state.move_history.push_back(action);

		return true;
	}
	else
	{
		return false;
	}
}

bool go::engine::is_empty_cell(Cell cell)
{
	return cell == Cell::EMPTY;
}

bool go::engine::is_empty_cell(const BoardState& state, uint32_t idx)
{
	return is_empty_cell(state.board[idx]);
}

void go::engine::calculate_score(
    const BoardState& boardState, Player& white_player, Player& black_player)
{
	uint32_t white_territory_score = 0, black_territory_score = 0,
	         score_temp = 0;

	// To detect wether the traversed territory belong to which side, "01" for
	// white and "10" for black
	unsigned char white = 0b00000001;
	unsigned char black = 0b00000010;
	unsigned char territory_type; // if the output of the traversed territory
	                              // was "11" the it was a false territory
	bool visited[BoardState::MAX_NUM_CELLS] = {
	    false}; // to avoid starting traversing a new territory from an already
	            // traversed empty cell

	// Traversing the board to detect any start of any territory
	for (uint32_t i = 0; i < boardState.MAX_BOARD_SIZE; i++)
	{
		for (uint32_t j = 0; j < boardState.MAX_BOARD_SIZE; j++)
		{
			territory_type = 0b00000000;
			score_temp = 0;
			if (is_empty_cell(boardState(i, j)) &&
			    !visited[boardState.index(i, j)])
				score_temp =
				    territory_points(boardState, territory_type, i, j, visited);

			// XNORing the territory_type to figure out the output of the
			// traversed territory
			if (territory_type == white)
				white_territory_score += score_temp;
			else if (territory_type == black)
				black_territory_score += score_temp;
		}
	}

	// Updating scores
	white_player.total_score = white_territory_score +
	                           white_player.number_alive_stones +
	                           white_player.number_captured_enemies;
	black_player.total_score = black_territory_score +
	                           black_player.number_alive_stones +
	                           black_player.number_captured_enemies;
}

uint32_t go::engine::territory_points(
    const BoardState& boardState, unsigned char& territory_type, uint32_t x,
    uint32_t y, bool* visited)
{

	uint32_t score = 0;
	unsigned char white = 0b00000001;
	unsigned char black = 0b00000010;
	visited[boardState.index(x, y)] = true;

	// Checking upper cell territory
	if (y != 0)
	{
		if (is_empty_cell(boardState(x, y - 1)) &&
		    !visited[boardState.index(x, y - 1)])
			score +=
			    territory_points(boardState, territory_type, x, y - 1, visited);
		else if (boardState(x, y - 1) == Cell::WHITE)
			territory_type |= white;
		else if (boardState(x, y - 1) == Cell::BLACK)
			territory_type |= black;
	}

	// Checking right cell territory
	if (x != boardState.MAX_BOARD_SIZE - 1)
	{
		if (is_empty_cell(boardState(x + 1, y)) &&
		    !visited[boardState.index(x + 1, y)])
			score +=
			    territory_points(boardState, territory_type, x + 1, y, visited);
		else if (boardState(x + 1, y) == Cell::WHITE)
			territory_type |= white;
		else if (boardState(x + 1, y) == Cell::BLACK)
			territory_type |= black;
	}

	// Checking bottom cell territory
	if (y != boardState.MAX_BOARD_SIZE - 1)
	{
		if (is_empty_cell(boardState(x, y + 1)) &&
		    !visited[boardState.index(x, y + 1)])
			score +=
			    territory_points(boardState, territory_type, x, y + 1, visited);
		else if (boardState(x, y + 1) == Cell::WHITE)
			territory_type |= white;
		else if (boardState(x, y + 1) == Cell::BLACK)
			territory_type |= black;
	}

	// Checking left cell territory
	if (x != 0)
	{
		if (is_empty_cell(boardState(x - 1, y)) &&
		    !visited[boardState.index(x - 1, y)])
			score +=
			    territory_points(boardState, territory_type, x - 1, y, visited);
		else if (boardState(x - 1, y) == Cell::WHITE)
			territory_type |= white;
		else if (boardState(x - 1, y) == Cell::BLACK)
			territory_type |= black;
	}

	score++;
	return score;
}

bool go::engine::is_overtime(const Player& player)
{
	return player.elapsed_time > player.allowed_time;
}

bool go::engine::is_game_finished(const GameState& state)
{
	if (state.move_history.size() > 1)
	{
		const Action& last = state.move_history.back();
		const Action& before_last = state.move_history.rbegin()[1];
		if (is_pass(last) && is_pass(before_last))
			return true;
	}
	else if (is_overtime(state.players[0]))
		return true;
	else if (is_overtime(state.players[1]))
		return true;

	bool full_board = true;
	for (Cell cell : state.board_state.board)
	{
		if (is_empty_cell(cell))
		{
			full_board = false;
			break;
		}
	}
	if (full_board)
		return true;

	return false;
}
