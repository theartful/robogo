#include <algorithm>
#include <array>
#include <functional>
#include <iomanip>
#include <iostream>
#include <utility>

#include "engine/board.h"
#include "engine/engine.h"
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
	auto& board = game.board;

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
		if (board(row, col) == engine::Stone::Black)
			return 'X';
		else if (board(row, col) == engine::Stone::White)
			return 'O';
		else if (board(row, col) == engine::Stone::Empty)
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
	for (int32_t row = board.size - 1; row >= 0; row--)
	{
		uint32_t urow = static_cast<uint32_t>(row);
		oss << ' ';
		oss << std::setw(2) << urow + 1 << ' ';

		for (uint32_t col = 0; col < board.size; col++)
			oss << point_symbol(urow, col) << ' ';

		oss << urow + 1 << std::endl;
	}
	show_col_names(oss, board.size);
	return oss.str();
}

expected<void> GTPController::play(Color color, Vertex vertex)
{
	if (engine::make_move(game, to_action(color, vertex)))
		return {};
	else
		return unexpected("illegal move"s);
}

void GTPController::clear_board()
{
	auto old_board_size = game.board.size;
	game = engine::GameState(old_board_size);
}

void GTPController::komi(float new_komi)
{
	rules.komi = new_komi;
}

expected<void> GTPController::boardsize(uint32_t size)
{
	if (size > 19 || size < 4)
		return unexpected("unacceptable size"s);
	game = engine::GameState(size);
	return {};
}

void GTPController::quit()
{
	quit_flag = true;
}

} // namespace go::gtp
