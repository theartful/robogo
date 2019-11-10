#include <array>
#include <stdint.h>
#include <vector>
#include <assert.h>


enum class Cell : unsigned char
{
	EMPTY,
	WHITE,
	BLACK,
};


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
