#ifndef _BOARD_H_
#define _BOARD_H_

#include <chrono>
#include <stdint.h>

#ifndef NDEBUG
#include <algorithm>
#include <iterator>
#endif

namespace go
{
	namespace engine
	{
		enum CellBits : unsigned char
		{
			WHITE_BIT = 0b00000001,
			BLACK_BIT = 0b00000010,
			ILLEGAL_WHITE_BIT = 0b00000100,
			ILLEGAL_BLACK_BIT = 0b00001000,
			DEAD_BIT = 0b00010000,
		};

		enum class Cell : unsigned char
		{
			WHITE = WHITE_BIT | ILLEGAL_WHITE_BIT | ILLEGAL_BLACK_BIT,
			BLACK = BLACK_BIT | ILLEGAL_WHITE_BIT | ILLEGAL_BLACK_BIT,
			DEAD = DEAD_BIT | ILLEGAL_WHITE_BIT | ILLEGAL_BLACK_BIT,
			DEAD_WHITE = DEAD_BIT | WHITE,
			DEAD_BLACK = DEAD_BIT | BLACK
		};

		enum class Player
		{
			WHITE,
			BLACK
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

		static inline Cell operator|(Cell cell, CellBits bit)
		{
			Cell result {static_cast<unsigned char>(cell) | bit};
#ifndef NDEBUG
			// in debug mode, enforce that Cell can't take wrong values
			static constexpr Cell cell_values[] = {
				Cell::WHITE, Cell::BLACK, Cell::DEAD, Cell::DEAD_WHITE, Cell::DEAD_BLACK
			};
			auto it = std::find(std::begin(cell_values), std::end(cell_values), result);
			if (it == std::end(cell_values))
			{
				fprintf(stderr, "Illegal cell transition from %02hhx to %02hhx!\n", cell, result);
				exit(-1);
			}
#endif
			return result;
		}
	}
}

#endif // _BOARD_H_
