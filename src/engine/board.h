#ifndef SRC_ENGINE_BOARD_H_
#define SRC_ENGINE_BOARD_H_

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
	EMPTY = 0,
	WHITE = 1,
	BLACK = 2,
	BORDER = 4
};

static constexpr Cell PLAYERS[] = {Cell::BLACK, Cell::WHITE};

struct BoardState
{
	static constexpr uint32_t MAX_BOARD_SIZE = 19;
	static constexpr uint32_t EXTENDED_BOARD_SIZE = MAX_BOARD_SIZE + 2;
	static constexpr uint32_t MAX_NUM_CELLS =
	    EXTENDED_BOARD_SIZE * EXTENDED_BOARD_SIZE;
	static constexpr uint32_t INVALID_INDEX = MAX_NUM_CELLS;

	uint32_t size;
	std::array<Cell, MAX_NUM_CELLS> board;
	uint32_t ko;

	BoardState(uint32_t size_ = MAX_BOARD_SIZE) : size(size_), ko{INVALID_INDEX}
	{
		std::fill(board.begin(), board.end(), Cell::EMPTY);
		for (uint32_t i = 0; i < EXTENDED_BOARD_SIZE; i++)
		{
			board[i] = Cell::BORDER;
			board[i + (EXTENDED_BOARD_SIZE - 1) * EXTENDED_BOARD_SIZE] =
			    Cell::BORDER;
			board[i * EXTENDED_BOARD_SIZE] = Cell::BORDER;
			board[i * EXTENDED_BOARD_SIZE + EXTENDED_BOARD_SIZE - 1] =
			    Cell::BORDER;
		}
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
		return (i + 1) * EXTENDED_BOARD_SIZE + (j + 1);
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

// A cluster is a maximal set of connected stones
struct Cluster
{
	mutable uint32_t parent_idx;
	uint32_t player;
	uint32_t size;
	uint32_t num_liberties;
	std::bitset<BoardState::MAX_NUM_CELLS> liberties_map;
};

// A union find structure
struct ClusterTable
{
	std::array<Cluster, BoardState::MAX_NUM_CELLS> clusters;
	ClusterTable() : clusters{} // initialize clusters to 0
	{
	}
};

struct Player
{
	uint32_t number_captured_enemies;
	uint32_t number_alive_stones;
	float total_score;

	Player()
	    : number_captured_enemies{0}, number_alive_stones{0}, total_score{0}
	{
	}
};

struct GameState
{
	BoardState board_state;
	ClusterTable cluster_table;
	uint32_t number_played_moves;
	uint32_t player_turn;
	std::array<Player, 2> players;
	std::vector<Action> move_history;

	GameState() : board_state(), number_played_moves{0}, player_turn{0}
	{
	}
};

// TODO: Support more rules and make it runtime!
struct Rules
{
	float komi = 6.5;
};

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_BOARD_H_
