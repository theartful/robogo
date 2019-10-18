#ifndef SRC_ENGINE_BOARD_H_
#define SRC_ENGINE_BOARD_H_

#include <stdint.h>

#include <assert.h>
#include <chrono>

namespace go
{
namespace engine
{
enum CellBits : unsigned char
{
	WHITE_BIT = 0b00000001,
	BLACK_BIT = 0b00000010,
	DEAD_BIT = 0b00010000,
	SUICIDE_WHITE_BIT = 0b01000000,
	SUICIDE_BLACK_BIT = 0b00100000,
};
static constexpr CellBits SUICIDE_BITS[] = {SUICIDE_BLACK_BIT, SUICIDE_WHITE_BIT};
static constexpr CellBits PLAYERS[] = {BLACK_BIT, WHITE_BIT};


enum class Cell : unsigned char
{
	EMPTY = 0,
	WHITE = WHITE_BIT,
	BLACK = BLACK_BIT,
	DEAD_WHITE = DEAD_BIT | WHITE,
	DEAD_BLACK = DEAD_BIT | BLACK,
	SUICIDE_WHITE = SUICIDE_WHITE_BIT,
	SUICIDE_BLACK = SUICIDE_BLACK_BIT
};

struct BoardState
{
	static constexpr uint32_t MAX_BOARD_SIZE = 19;
	Cell board[MAX_BOARD_SIZE * MAX_BOARD_SIZE];

	Cell& operator()(uint32_t i, uint32_t j)
	{
		assert(i < MAX_BOARD_SIZE);
		assert(j < MAX_BOARD_SIZE);
		return board[i * MAX_BOARD_SIZE + j];
	}

	Cell operator()(uint32_t i, uint32_t j) const
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
	uint32_t player_index;
};

struct Player
{
	uint32_t number_captured_enemies;
	uint32_t number_alive_stones;
	uint32_t total_score;
	// maximum time allowed for a player throughout the game
	std::chrono::duration<uint32_t, std::milli> max_duration;
	// the time from which it's this player's move
	// should be updated each time it's his turn
	std::chrono::steady_clock::time_point move_start_time;
	// should be updated when the player finishes his move
	// time_left -= duration(now - move_start_time);
	std::chrono::duration<uint32_t, std::milli> time_left;
};

struct GameState
{
	BoardState board_state;
	uint32_t board_size;
	uint32_t number_played_moves;
	uint32_t player_turn;
	Player players[2];
};


static inline Cell operator|(Cell cell, CellBits bit)
{
	return static_cast<Cell>(static_cast<unsigned char>(cell) | bit);
}

static inline unsigned char operator&(Cell cell, unsigned char bit)
{
	return (static_cast<unsigned char>(cell) | bit);
}

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_BOARD_H_
