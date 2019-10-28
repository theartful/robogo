#ifndef SRC_ENGINE_BOARD_H_
#define SRC_ENGINE_BOARD_H_

#include <array>
#include <assert.h>
#include <chrono>
#include <stdint.h>

#include "config.h"

namespace go
{
namespace engine
{
enum class Cell : unsigned char
{
	EMPTY,
	WHITE,
	BLACK,
};

static constexpr Cell PLAYERS[] = {Cell::BLACK, Cell::WHITE};

struct BoardState
{
	static constexpr uint32_t MAX_BOARD_SIZE = 19;
	static constexpr uint32_t MAX_NUM_CELLS = MAX_BOARD_SIZE * MAX_BOARD_SIZE;
	static constexpr uint32_t INVALID_INDEX = MAX_NUM_CELLS;

	std::array<Cell, MAX_NUM_CELLS> board;
	uint32_t ko;

	BoardState() : board{Cell::EMPTY}, ko{INVALID_INDEX}
	{
	}

	Cell& operator()(uint32_t i, uint32_t j)
	{
		assert(i < MAX_BOARD_SIZE);
		assert(j < MAX_BOARD_SIZE);
		return board[index(i, j)];
	}

	Cell operator()(uint32_t i, uint32_t j) const
	{
		assert(i < MAX_BOARD_SIZE);
		assert(j < MAX_BOARD_SIZE);
		return board[index(i, j)];
	}

	static uint32_t index(uint32_t i, uint32_t j)
	{
		return i * MAX_BOARD_SIZE + j;
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
	uint32_t player_index; // 0 for white, 1 for black
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

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_BOARD_H_
