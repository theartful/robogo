#include "simplegui.h"
#include "engine/utility.h"
#include <algorithm>
#include <iostream>
#include <stdint.h>
using namespace go::simplegui;
using namespace go::engine;

BoardSimpleGUI::BoardSimpleGUI()
{
}

// Blocks waiting for input from user, and return when he enters position
uint32_t BoardSimpleGUI::generate_move(const Game& game)
{
	std::string command;
	uint32_t x, y;

	print_game_state(game);

	while (true)
	{
		std::cout << "Enter command: ";
		std::cin >> command;

		to_lower(command);

		if (command == "board")
		{
			print_board(
			    game.get_board_state(), game.get_game_state().player_turn);
		}
		else if (command == "mv")
		{
			read_position(x, y);
			if (x == UINT32_MAX && y == UINT32_MAX) // pass
				return BoardState::INVALID_INDEX;
			else if (x == UINT32_MAX) // invalid input
				continue;
			else
				return BoardState::index(x, y);
		}
		else if (command == "lib")
		{
			read_position(x, y);
			if (x == UINT32_MAX) // invalid input
				continue;
			uint32_t index = BoardState::index(x, y);
			print_liberties(get_cluster(game.get_cluster_table(), index));
		}
		else if (command == "cluster")
		{
			read_position(x, y);
			if (x == UINT32_MAX) // invalid input
				continue;
			uint32_t index = BoardState::index(x, y);
			print_board(
			    game.get_board_state(), game.get_game_state().player_turn);
			print_cluster_info(
			    index, game.get_cluster_table(),
			    game.get_game_state().board_state);
		}
		else if (command == "state")
		{
			print_game_state(game);
		}
		else if (command == "score")
		{
			const GameState& game_state = game.get_game_state();
			auto [black_score, white_score] =
			    engine::calculate_score(game_state);
			std::cout << "Black total score: " << black_score << '\n';
			std::cout << "White total score: " << white_score << '\n';
		}
		else if (command == "mvv")
		{
			uint32_t pos = 0;
			std::cin >> pos;
			return pos;
		}
	}
}

void BoardSimpleGUI::read_position(uint32_t& x, uint32_t& y)
{
	std::string position;
	std::cin >> position;

	to_lower(position);

	if (position == "pass")
	{
		x = UINT32_MAX;
		y = UINT32_MAX;
		return;
	}

	char column = position[0];
	uint32_t row = uint32_t(atoi(position.substr(1).c_str()));

	get_index(column, row, x, y);
}

void BoardSimpleGUI::to_lower(std::string& str)
{
	for_each(str.begin(), str.end(), [](char& c) { c = std::tolower(c); });
}

void BoardSimpleGUI::get_index(
    char column, uint32_t row, uint32_t& x, uint32_t& y)
{
	if (column < 'a' || column > 't' || column == 'i' || row < 1 || row > 19)
	{
		x = UINT32_MAX;
		y = 0;
		return;
	}

	const char MIN_CHAR = 'a';

	// board doesn't have letter i, so I decrement column to remove the gap
	if (column >= 'j')
		column--;

	x = BOARD_SIZE - row;
	y = uint32_t(column - MIN_CHAR);
}

// clears console
void BoardSimpleGUI::clear_screen()
{
	// Hacky solution, but crossplatform :D
	// std::cout << "\x1B[2J\x1B[H";
}

inline bool BoardSimpleGUI::is_special(uint32_t idx)
{
	return idx == 3 || idx == 9 || idx == 15;
}

char BoardSimpleGUI::get_board_symbol(Cell cell, uint32_t x, uint32_t y)
{

	if (cell == Cell::WHITE)
		return '@';
	else if (cell == Cell::BLACK)
		return '#';
	else if (is_special(x) && is_special(y))
		return '_';
	else if (cell == Cell::BORDER)
		return 'B';
	else
		return '.';
}

void BoardSimpleGUI::print_board(const BoardState& board, uint32_t player_turn)
{
	clear_screen();
	std::cout << "\t\t   ";
	for (char i = 'A'; i <= 'T'; ++i)
	{
		if (i == 'I') // for some reason, go boatd doesn't have letter i
			continue;
		std::cout << i << " ";
	}

	std::cout << std::endl;

	const uint32_t LAST_TWO_DIGIT_NUMBER = 10;

	for (uint32_t i = 0; i < BOARD_SIZE; ++i)
	{
		std::cout << "\t  \t";
		if (BOARD_SIZE - i < LAST_TWO_DIGIT_NUMBER)
			std::cout << " ";

		std::cout << BOARD_SIZE - i << " ";

		for (uint32_t j = 0; j < BOARD_SIZE; ++j)
			std::cout << get_board_symbol(board(i, j), i, j) << " ";

		std::cout << BOARD_SIZE - i << std::endl;
	}

	std::cout << "\t\t   ";
	for (char i = 'A'; i <= 'T'; ++i)
	{
		if (i == 'I') // for some reason, go boatd doesn't have letter i
			continue;
		std::cout << i << " ";
	}

	std::cout << std::endl;
	std::cout << "Players turn: " << player_turn << std::endl;
}

void BoardSimpleGUI::print_game_state(const Game& game)
{
	const GameState& game_state = game.get_game_state();
	clear_screen();
	print_board(game_state.board_state, game_state.player_turn);

	std::cout << "Allowed time: "
	          << game.get_allowed_time(game_state.player_turn).count() << " ";
	std::cout << "Elapsed time: "
	          << game.get_elapsed_time(game_state.player_turn).count()
	          << std::endl;

	std::cout << "Number of played moves: " << game_state.number_played_moves
	          << std::endl;

	std::cout << "\tPlayer 0 \t\t\t Player 1" << std::endl;

	std::cout << "Number of Captured: "
	          << game_state.players[0].number_captured_enemies;
	std::cout << "\t\t\tNumber of Captured: "
	          << game_state.players[1].number_captured_enemies << std::endl;

	std::cout << "Number of Alive: "
	          << game_state.players[0].number_alive_stones;
	std::cout << "\t\t\tNumber of Alive: "
	          << game_state.players[1].number_alive_stones << std::endl;
}

void BoardSimpleGUI::print_liberties(const Cluster& cluster)
{
	clear_screen();
	std::cout << "Liberty map" << std::endl;
	std::cout << "\t\t   ";
	for (char i = 'A'; i <= 'T'; ++i)
	{
		if (i == 'I')
			continue;
		std::cout << i << " ";
	}

	std::cout << std::endl;

	const uint32_t LAST_TWO_DIGIT_NUMBER = 10;

	for (uint32_t i = 0; i < BOARD_SIZE; ++i)
	{

		std::cout << "\t  \t";
		if (BOARD_SIZE - i < LAST_TWO_DIGIT_NUMBER)
			std::cout << " ";

		std::cout << BOARD_SIZE - i << " ";

		for (uint32_t j = 0; j < BOARD_SIZE; ++j)
		{
			if (cluster.liberties_map[BoardState::index(i, j)])
				std::cout << cluster.liberties_map[BoardState::index(i, j)]
				          << " ";
			else
				std::cout << "-"
				          << " ";
		}
		std::cout << BOARD_SIZE - i << std::endl;
	}

	std::cout << "\t\t   ";
	for (char i = 'A'; i <= 'T'; ++i)
	{
		if (i == 'I') // for some reason, go boatd doesn't have letter i
			continue;
		std::cout << i << " ";
	}

	std::cout << std::endl;
}

void BoardSimpleGUI::print_cluster_info(
    uint32_t index, const ClusterTable& table,
    const go::engine::BoardState& state)
{
	const Cluster& cluster = get_cluster(table, index);

	std::cout << "Parent idx = " << cluster.parent_idx;

	uint32_t x = cluster.parent_idx / BOARD_SIZE;
	uint32_t y = cluster.parent_idx % BOARD_SIZE;

	std::cout << " = " << get_alphanumeric_position(x, y) << " ";
	std::cout << "Player idx = " << cluster.player << std::endl;
	std::cout << "Cluster size = " << cluster.size << std::endl;
	std::cout << "Number of liberties = " << cluster.num_liberties << std::endl;
	std::cout << "Cluster positions: " << std::endl;
	std::cout << "[";

	bool is_first_number = true;
	for_each_cluster_cell(cluster, state, [&](uint32_t idx) {
		x = idx / BOARD_SIZE;
		y = idx % BOARD_SIZE;

		if (!is_first_number)
			std::cout << ", ";
		std::cout << get_alphanumeric_position(x, y);
		is_first_number = false;
	});

	std::cout << "]" << std::endl;
}

std::string BoardSimpleGUI::get_alphanumeric_position(uint32_t x, uint32_t y)
{
	uint32_t row = BOARD_SIZE - x;
	char column = 'A' + y;
	if (column >= 'I')
		column++;
	return column + std::to_string(row);
}

std::string BoardSimpleGUI::get_alphanumeric_position(uint32_t pos)
{
	return get_alphanumeric_position((pos / 21) - 1, pos % 21 - 1);
}
