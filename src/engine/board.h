#ifndef SRC_ENGINE_BOARD_H_
#define SRC_ENGINE_BOARD_H_

#include "config.h"
#include <array>
#include <assert.h>
#include <bitset>
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
	BLACK,
	WHITE,
	BORDER
};

static constexpr Cell PLAYERS[] = {Cell::BLACK, Cell::WHITE};

struct BoardState
{
	static constexpr uint32_t MAX_BOARD_SIZE = 19;
	static constexpr uint32_t EXTENDED_BOARD_SIZE = MAX_BOARD_SIZE + 2;
	static constexpr uint32_t MAX_NUM_CELLS =
	    EXTENDED_BOARD_SIZE * EXTENDED_BOARD_SIZE;
	static constexpr uint32_t INVALID_INDEX = MAX_NUM_CELLS;
	static constexpr uint32_t BOARD_BEGIN = EXTENDED_BOARD_SIZE + 1;
	static constexpr uint32_t BOARD_END =
	    (MAX_BOARD_SIZE + 1) * EXTENDED_BOARD_SIZE + EXTENDED_BOARD_SIZE;

	std::array<Cell, MAX_NUM_CELLS> board;
	std::array<uint16_t, MAX_NUM_CELLS> neighbor_count;
	std::array<uint16_t, MAX_NUM_CELLS> empty_cells;
	uint32_t num_empty;
	uint32_t ko;

	BoardState() : num_empty{0}, ko{INVALID_INDEX}
	{
		std::fill(board.begin(), board.end(), Cell::EMPTY);
		std::fill(neighbor_count.begin(), neighbor_count.end(), 4);
		for (uint32_t i = 0; i < EXTENDED_BOARD_SIZE; i++)
		{
			board[i] = Cell::BORDER;
			board[i + (EXTENDED_BOARD_SIZE - 1) * EXTENDED_BOARD_SIZE] =
			    Cell::BORDER;
			board[i * EXTENDED_BOARD_SIZE] = Cell::BORDER;
			board[i * EXTENDED_BOARD_SIZE + EXTENDED_BOARD_SIZE - 1] =
			    Cell::BORDER;

			neighbor_count[index(0, i)] = 3;
			neighbor_count[index(i, 0)] = 3;
			neighbor_count[index(MAX_BOARD_SIZE - 1, i)] = 3;
			neighbor_count[index(i, MAX_BOARD_SIZE - 1)] = 3;
		}
		for (uint32_t i = 0; i < MAX_BOARD_SIZE; i++)
		{
			for (uint32_t j = 0; j < MAX_BOARD_SIZE; j++)
			{
				empty_cells[num_empty++] = index(i, j);
			}
		}
		neighbor_count[index(0, 0)] = 2;
		neighbor_count[index(MAX_BOARD_SIZE - 1, 0)] = 2;
		neighbor_count[index(0, MAX_BOARD_SIZE - 1)] = 2;
		neighbor_count[index(MAX_BOARD_SIZE - 1, MAX_BOARD_SIZE - 1)] = 2;
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

	static constexpr uint32_t index(uint32_t i, uint32_t j)
	{
		return (i + 1) * EXTENDED_BOARD_SIZE + (j + 1);
	}

	static constexpr uint32_t get_row(uint32_t pos)
	{
		return pos / EXTENDED_BOARD_SIZE - 1;
	}

	static constexpr uint32_t get_column(uint32_t pos)
	{
		return pos % EXTENDED_BOARD_SIZE - 1;
	}
};

inline bool is_empty_cell(Cell cell)
{
	return cell == Cell::EMPTY;
}
inline bool is_empty_cell(const BoardState& state, uint32_t idx)
{
	return is_empty_cell(state.board[idx]);
}

inline void increment_empty_count(BoardState& state, uint32_t idx)
{
	state.neighbor_count[idx] += 1;
}
inline void decrement_empty_count(BoardState& state, uint32_t idx)
{
	state.neighbor_count[idx] -= 1;
}
inline void increment_cell_count(BoardState& state, Cell cell, uint32_t idx)
{
	state.neighbor_count[idx] += (1UL << (3 * static_cast<uint16_t>(cell)));
}
inline void decrement_cell_count(BoardState& state, Cell cell, uint32_t idx)
{
	state.neighbor_count[idx] -= (1UL << (3 * static_cast<uint16_t>(cell)));
}
inline uint16_t get_white_count(const BoardState& state, uint32_t idx)
{
	return (state.neighbor_count[idx] & uint16_t(0b000111000)) >> 3;
}
inline uint16_t get_black_count(const BoardState& state, uint32_t idx)
{
	return (state.neighbor_count[idx] & uint16_t(0b111000000)) >> 6;
}
inline uint16_t get_empty_count(const BoardState& state, uint32_t idx)
{
	return (state.neighbor_count[idx] & uint16_t(0b000000111));
}
inline void add_empty_cell(BoardState& state, uint32_t idx)
{
	state.empty_cells[state.num_empty++] = idx;
}
inline void remove_empty_cell(BoardState& state, uint32_t idx)
{
	for (uint32_t i = 0; i < state.num_empty; i++)
	{
		if (state.empty_cells[i] == idx)
		{
			std::swap(
			    state.empty_cells[i], state.empty_cells[--state.num_empty]);
			return;
		}
	}
}

struct Action
{
	static constexpr uint32_t PASS = BoardState::INVALID_INDEX;
	static constexpr uint32_t INVALID_ACTION = PASS + 1;
	uint32_t pos;
	uint32_t player_index;
};

inline bool is_pass(const Action& action)
{
	return action.pos == Action::PASS;
}

inline bool is_invalid(const Action& action)
{
	return action.pos >= Action::INVALID_ACTION;
}

// A cluster is a maximal set of connected stones
struct Cluster
{
	mutable uint16_t parent_idx;
	// TODO: decide whether to use parent tail to quickly iterate cluster stones
	// or leave the original implementation
	// uint32_t tail;
	uint16_t player;
	uint16_t size;
	uint16_t num_liberties;
	uint16_t atari_lib;
	std::bitset<BoardState::MAX_NUM_CELLS> liberties_map;

	Cluster()
	    : parent_idx{0},
	      /*tail{0},*/ player{0}, size{0}, num_liberties{0}, atari_lib{0}
	{
	}
};

// A union find structure
struct ClusterTable
{
	std::array<Cluster, BoardState::MAX_NUM_CELLS> clusters;
	// std::array<uint32_t, BoardState::MAX_NUM_CELLS> next_cell;
	uint32_t num_in_atari;
	ClusterTable() : clusters{}, /*next_cell{},*/ num_in_atari{0}
	{
	}
};

struct Player
{
	uint32_t number_captured_enemies;
	uint32_t number_alive_stones;

	Player() : number_captured_enemies{0}, number_alive_stones{0}
	{
	}
};

struct GameState
{
	BoardState board_state;
	ClusterTable cluster_table;
	uint32_t board_size;
	uint32_t player_turn;
	std::array<Player, 2> players;
	std::vector<Action> move_history;

	GameState()
	    : board_state(), board_size{BoardState::MAX_BOARD_SIZE}, player_turn{0}
	{
	}
};

// TODO: Support more rules and make it runtime!
struct Rules
{
	static constexpr float KOMI = 6.5;
};

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_BOARD_H_
