#ifndef _STATE_H_
#define _STATE_H_

#include <chrono>
#include <stdint.h>

namespace go
{
	namespace engine
	{
		enum class Player
		{
			WHITE_PLAYER,
			BLACK_PLAYER
		};

		enum CellBits : unsigned char
		{
			WHITE_BIT = 0b00000001,
			BLACK_BIT = 0b00000010,
			ILLEGAL_WHITE_BIT = 0b00000100,
			ILLEGAL_BLACK_BIT = 0b00001000,
			DEAD_BIT = 0b00010000,
		};

		enum Cell : unsigned char
		{
			WHITE = WHITE_BIT | ILLEGAL_WHITE_BIT | ILLEGAL_BLACK_BIT,
			BLACK = BLACK_BIT | ILLEGAL_WHITE_BIT | ILLEGAL_BLACK_BIT,
			DEAD = DEAD_BIT | ILLEGAL_WHITE_BIT | ILLEGAL_BLACK_BIT,
			DEAD_WHITE = DEAD_BIT | WHITE,
			DEAD_BLACK = DEAD_BIT | BLACK
		};

		struct BoardState
		{
			static constexpr uint32_t MAX_BOARD_SIZE = 19;

			Cell board[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
			uint32_t available_white_moves;
			uint32_t available_black_moves;
		};

		struct Action
		{
			uint32_t x;
			uint32_t y;
			Player p;
		};

		struct GameState
		{
			BoardState board_state;
			uint32_t board_size;
			uint32_t number_played_moves;
			Player player_turn;
			// score
			uint32_t white_score;
			uint32_t black_score;
			// time information
			std::chrono::steady_clock::time_point start_time;
			std::chrono::duration<uint32_t, std::milli> duration;
		};
	}
}

#endif // _STATE_H_