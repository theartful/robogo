#ifndef SRC_ENGINE_BOARD_H_
#define SRC_ENGINE_BOARD_H_

#include <stdint.h>

#include <assert.h>
#include <chrono>

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
	DEAD_BIT = 0b00010000,
};

enum class Cell : unsigned char
{
	EMPTY = 0,
	WHITE = WHITE_BIT,
	BLACK = BLACK_BIT,
	DEAD_WHITE = DEAD_BIT | WHITE,
	DEAD_BLACK = DEAD_BIT | BLACK,
};

enum class Player : unsigned char
{
	WHITE = WHITE_BIT,
	BLACK = BLACK_BIT
};

struct BoardState
{
	static constexpr uint32_t MAX_BOARD_SIZE = 19;

	Cell board[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
	uint32_t available_white_moves;
	uint32_t available_black_moves;

	Cell& operator()(uint32_t i, uint32_t j)
	{
		assert(i < MAX_BOARD_SIZE);
		assert(j < MAX_BOARD_SIZE);
		return board[i * MAX_BOARD_SIZE + j];
	}
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
	return static_cast<Cell>(static_cast<unsigned char>(cell) | bit);
}

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_BOARD_H_
