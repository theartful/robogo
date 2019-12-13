#include "gtp.h"
using namespace gtp;

// Debug Commands
inline bool GTPEngine::is_special(uint32_t idx)
{
	return idx == 3 || idx == 9 || idx == 15;
}

char GTPEngine::get_board_symbol(go::engine::Cell cell, uint32_t x, uint32_t y)
{
	if (cell == go::engine::Cell::WHITE)
		return '@';
	else if (cell == go::engine::Cell::BLACK)
		return '#';
	else if (is_special(x) && is_special(y))
		return '_';
	else if (cell == go::engine::Cell::BORDER)
		return 'B';
	else
		return '.';
}

/**
 * @param   none
 * @returns {string}   A diagram of the board position
 */
std::string GTPEngine::showboard()
{
    const auto game_state = game.get_game_state();
    auto board = game_state.board_state;
    auto player_turn = game_state.player_turn;

    std::string board_str = "\n\t\t   ";
	for (char i = 'A'; i <= 'T'; ++i)
	{
		if (i == 'I') // for some reason, go boatd doesn't have letter i
			continue;

        board_str.push_back(i);
        board_str.push_back(' ');
	}
    board_str.push_back('\n');

	const uint32_t LAST_TWO_DIGIT_NUMBER = 10;

	for (uint32_t i = 0; i < BOARD_SIZE; ++i)
	{
		board_str += "\t  \t";
		if (BOARD_SIZE - i < LAST_TWO_DIGIT_NUMBER)
			board_str.push_back(' ');

        board_str += std::to_string(BOARD_SIZE - i);
        board_str.push_back(' ');

		for (uint32_t j = 0; j < BOARD_SIZE; ++j)
        {
            board_str.push_back(get_board_symbol(board(i, j), i, j));
            board_str.push_back(' ');
        }

        board_str += std::to_string(BOARD_SIZE - i);
        board_str.push_back('\n');
	}

	board_str += "\t\t   ";
	for (char i = 'A'; i <= 'T'; ++i)
	{
		if (i == 'I') // for some reason, go boatd doesn't have letter i
			continue;

        board_str.push_back(i);
        board_str.push_back(' ');
	}

    return board_str;
}
