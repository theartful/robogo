#ifndef SRC_ENGINE_BOARD_H_
#define SRC_ENGINE_BOARD_H_

#include <array>
#include <assert.h>
#include <chrono>
#include <stdint.h>
#include <vector>

#ifndef NDEBUG
#include <stdio.h>
#define DEBUG_PRINT(...)                                                       \
	do                                                                         \
	{                                                                          \
		printf(__VA_ARGS__);                                                   \
	} while (0)
#else
#define DEBUG_PRINT(...) (void)0
#endif // NDEBUG

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
	static constexpr uint32_t PASS = BoardState::INVALID_INDEX;
	uint32_t pos;
	uint32_t player_index;
};

inline bool is_pass(const Action& action)
{
	return action.pos == Action::PASS;
}

inline bool is_invalid(const Action& action)
{
	return action.pos > BoardState::INVALID_INDEX || action.player_index > 1;
}

struct Player
{
	static constexpr uint32_t DEFAULT_ALLOWED_TIME = 15 * 60 * 1000;
	uint32_t number_captured_enemies;
	uint32_t number_alive_stones;
	uint32_t total_score;
	uint32_t player_index; // 0 for black, 1 for white
	// maximum time allowed for a player throughout the game
	std::chrono::duration<uint32_t, std::milli> allowed_time;
	// the time from which it's this player's move
	// should be updated each time it's his turn
	std::chrono::steady_clock::time_point move_start_time;
	// should be updated when the player finishes his move
	// elapsed_time += duration(now - move_start_time);
	std::chrono::duration<uint32_t, std::milli> elapsed_time;

	Player(uint32_t idx = 0)
	    : number_captured_enemies{0}, number_alive_stones{0}, total_score{0},
	      player_index{idx}, allowed_time{DEFAULT_ALLOWED_TIME}, elapsed_time{0}
	{
	}
};

struct GameState
{
	BoardState board_state;
	uint32_t board_size;
	uint32_t number_played_moves;
	uint32_t player_turn;
	std::array<Player, 2> players;
	std::vector<Action> move_history;

	GameState()
	    : board_state(), board_size{BoardState::MAX_BOARD_SIZE},
	      number_played_moves{0}, player_turn{0}, players{0, 1}
	{
	}
};

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_BOARD_H_
