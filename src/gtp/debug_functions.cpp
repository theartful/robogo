#include "sgf/sgf.h"
#include "engine/board.h"
#include "engine/engine.h"
#include "engine/liberties.h"
#include "engine/score.h"
#include "gtp/gtp.h"
#include "gtp/utility.h"

#include <algorithm>
#include <array>
#include <functional>
#include <iomanip>
#include <fstream>

namespace go::gtp
{

static inline char get_extended_column_char(uint32_t col)
{
	return std::array{
		'?', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K',
		'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', '?'}[col];
}

std::string GTPController::rg_showboard()
{
	auto show_col_names = [](std::ostringstream& stream, uint32_t board_size) {
		stream << "    ";
		for (uint32_t col = 0; col < board_size; col++)
			stream << get_extended_column_char(col) << ' ';
		stream << std::endl;
	};

	auto point_symbol = [&](uint32_t row, uint32_t col) -> char {
		auto is_star_point = [](uint32_t r, uint32_t c) -> bool {
			return
				(r == 4 || r == 10 || r == 16) &&
				(c == 4 || c == 10 || c == 16);
		};
		if (get_stone(game, row, col) == engine::Stone::Black)
			return 'X';
		else if (get_stone(game, row, col) == engine::Stone::White)
			return 'O';
		else if (get_stone(game, row, col) == engine::Stone::Empty)
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

uint32_t GTPController::countlib(Vertex vertex)
{
	return count_liberties(game, vertex.index());
}

std::vector<Vertex> GTPController::findlib(Vertex vertex)
{
	std::vector<uint32_t> libs = get_liberties(game, vertex.index());
	std::vector<Vertex> output;
	output.reserve(libs.size());
	std::transform(libs.begin(), libs.end(), std::back_inserter(output),
			[] (uint32_t lib) { return Vertex(lib); });
	return output;
}

bool GTPController::is_legal(Color color, Vertex vertex)
{
	return go::engine::is_legal_move(game, to_action(color, vertex));
}

bool GTPController::is_suicide(Color color, Vertex vertex)
{
	return go::engine::is_suicide_move(game, to_action(color, vertex));
}

uint32_t GTPController::captures(Color color)
{
	return go::engine::num_captures(game, to_player_idx(color));
}

Color GTPController::color(Vertex vertex)
{
	switch (go::engine::get_stone(game, vertex.index()))
	{
	case engine::Stone::White:
		return Color::White;
	case engine::Stone::Black:
		return Color::Black;
	default:
		return Color::Empty;
	}
}

Color GTPController::loadsgf(std::string file, int32_t move_num)
{
	std::ifstream fin(file);
	std::string line;
	sgf::SGFParser parser(fin);
	auto tree = parser.parse();
	sgf::execute_sgf(tree, game, rules, move_num);
	fin.close();
	if (game.player_turn == 0)
		return Color::Black;
	else
		return Color::White;
}

Score GTPController::final_score()
{
	Score score;
	std::tie(score.black, score.white) = engine::calculate_score(game, rules);
	return score;
}

} // namespace go::gtp
