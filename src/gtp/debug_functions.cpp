#include <algorithm>
#include <array>
#include <functional>
#include <iomanip>
#include <iostream>

#include "engine/board.h"
#include "engine/engine.h"
#include "gtp/gtp.h"
#include "gtp/utility.h"

namespace go::gtp
{

static inline char get_extended_column_char(uint32_t col)
{
	return std::array{'?', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K',
	                  'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', '?'}[col];
}

std::string GTPController::rg_showboard()
{
	auto& board = game.board;

	auto show_col_names = [](std::ostringstream& stream, uint32_t board_size) {
		stream << "    ";
		for (uint32_t col = 0; col < board_size; col++)
			stream << get_extended_column_char(col) << ' ';
		stream << std::endl;
	};

	auto point_symbol = [&](uint32_t row, uint32_t col) -> char {
		auto is_star_point = [](uint32_t r, uint32_t c) -> bool {
			return (r == 4 || r == 10 || r == 16) &&
			       (c == 4 || c == 10 || c == 16);
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
	show_col_names(oss, 21);
	for (int32_t row = 20 - 1; row >= -1; row--)
	{
		uint32_t urow = static_cast<uint32_t>(row);
		oss << ' ';
		oss << std::setw(2) << urow + 1 << ' ';

		for (int32_t col = -1; col < 20; col++)
			oss << point_symbol(urow, static_cast<uint32_t>(col)) << ' ';

		oss << urow + 1 << std::endl;
	}
	show_col_names(oss, 21);
	return oss.str();
}

} // namespace go::gtp
