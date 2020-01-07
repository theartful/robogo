#include <algorithm>
#include <array>
#include <functional>
#include <utility>
#include <iostream>
#include <iomanip>

#include "engine/board.h"
#include "engine/interface.h"
#include "gtp/gtp.h"
#include "gtp/utility.h"

namespace go::gtp
{

using namespace std::string_literals;

uint32_t GTPController::protocol_version()
{
	return 2u;
}

std::string_view GTPController::name()
{
	return "RoboGo";
}

std::string_view GTPController::version()
{
	return "1.0.0";
}

bool GTPController::known_command(std::string_view command)
{
	return std::find(known_commands.begin(), known_commands.end(), command) !=
	       known_commands.end();
}

std::string GTPController::list_commands()
{
	std::ostringstream oss;
	auto it = known_commands.begin();
	oss << *(it++);
	for (; it != known_commands.end(); it++)
		oss << std::endl << *it;
	return oss.str();
}

std::string GTPController::showboard()
{
	auto& board = game.board_state;

	auto show_col_names = [](std::ostringstream& stream, uint32_t board_size) {
		stream << "    ";
		for (uint32_t col = 0; col < board_size; col++)
			stream << get_column_char(col) << ' ';
		stream << std::endl;
	};

	auto point_symbol = [&](uint32_t row, uint32_t col) -> char {
		auto is_star_point = [](uint32_t r, uint32_t c) -> bool {
			return (r == 3 || r == 9 || r == 15) &&
			       (c == 3 || c == 9 || c == 15);
		};
		if (board(row, col) == engine::Cell::BLACK)
			return 'X';
		else if (board(row, col) == engine::Cell::WHITE)
			return 'O';
		else if (board(row, col) == engine::Cell::EMPTY)
			if (is_star_point(row, col))
				return '+';
			else
				return '.';
		else
			return '?';
	};

	std::ostringstream oss;
	oss << std::endl;
	show_col_names(oss, board.size);
	for (uint32_t row = 0; row < board.size; row++)
	{
		oss << ' ';
		oss << std::setw(2) << (board.size - row) << ' ';

		for (uint32_t col = 0; col < board.size; col++)
			oss << point_symbol(row, col) << ' ';

		oss << (board.size - row) << std::endl;
	}
	show_col_names(oss, board.size);
	return oss.str();
}

expected<void> GTPController::play(Color color, Vertex vertex)
{
	uint32_t player_idx = color == Color::BLACK ? 0 : 1;
	uint32_t pos = engine::BoardState::index(vertex.row, vertex.col);

	std::cout << static_cast<int>(color) << '\t' << vertex.row << '\t'
	          << vertex.col << '\n';

	if (engine::make_move(game, engine::Action{player_idx, pos}))
		return {};
	else
		return unexpected("illegal move"s);
}

} // namespace go::gtp
