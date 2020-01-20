#ifndef _ENGINE_BOARD_H_
#define _ENGINE_BOARD_H_

#include "config.h"
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <vector>

#include "engine/utility.h"

namespace go::engine
{

enum class Stone : unsigned char
{
	Empty,
	Black,
	White,
	Border
};

inline constexpr Stone get_player_stone(uint16_t player_idx)
{
	constexpr std::array colors = {Stone::Black, Stone::White};
	return colors[player_idx];
}
inline constexpr uint32_t get_player_idx(Stone stone)
{
	constexpr std::array<uint32_t, 4> idxs = {3, 0, 1, 3};
	return idxs[static_cast<unsigned char>(stone)];
}

struct BoardState
{
	static constexpr auto MAX_SIZE = 19;
	static constexpr auto MAX_AREA = MAX_SIZE * MAX_SIZE;
	static constexpr auto EXTENDED_SIZE = MAX_SIZE + 2;
	static constexpr auto EXTENDED_AREA = EXTENDED_SIZE * EXTENDED_SIZE;
	static constexpr uint32_t INVALID_INDEX = EXTENDED_AREA;

	std::array<Stone, EXTENDED_AREA> stones;
	std::array<uint16_t, MAX_AREA> empty_stones;
	MarginRemapped2DArray<uint16_t, MAX_SIZE> neighbor_count;

	uint16_t num_empty_stones;
	uint16_t size;
	uint16_t ko;

	explicit BoardState(uint16_t size_)
	    : stones{}, empty_stones{}, neighbor_count{},
	      num_empty_stones{0}, size{size_}, ko{INVALID_INDEX}
	{
		const uint16_t extended_size = size + 2u;

		// not constexpr Q_Q
		std::fill(stones.begin(), stones.end(), Stone::Empty);
		std::fill(neighbor_count.begin(), neighbor_count.end(), 4);
		for (uint32_t i = 0; i < extended_size; i++)
		{
			stones[i] = Stone::Border;
			stones[i + (extended_size - 1u) * EXTENDED_SIZE] = Stone::Border;
			stones[i * EXTENDED_SIZE] = Stone::Border;
			stones[i * EXTENDED_SIZE + extended_size - 1u] = Stone::Border;
		}
		for (uint32_t i = 0; i < size; i++)
		{
			for (uint32_t j = 0; j < size; j++)
			{
				empty_stones[num_empty_stones++] = index(i, j);
			}
		}
		for (uint32_t i = 0; i < size; i++)
		{
			neighbor_count[index(0, i)] = 3;
			neighbor_count[index(i, 0)] = 3;
			neighbor_count[index(size - 1u, i)] = 3;
			neighbor_count[index(i, size - 1u)] = 3;
		}
		neighbor_count[index(0, 0)] = 2;
		neighbor_count[index(size - 1u, 0)] = 2;
		neighbor_count[index(0, size - 1u)] = 2;
		neighbor_count[index(size - 1u, size - 1u)] = 2;
	}
	BoardState() : BoardState{MAX_SIZE}
	{
	}
	constexpr Stone& operator()(uint32_t i, uint32_t j)
	{
		return stones[index(i, j)];
	}
	constexpr Stone operator()(uint32_t i, uint32_t j) const
	{
		return stones[index(i, j)];
	}
	static constexpr uint32_t index(uint32_t i, uint32_t j)
	{
		return (i + 1) * EXTENDED_SIZE + (j + 1);
	}
};

inline constexpr bool is_empty(Stone stone)
{
	return stone == Stone::Empty;
}
inline constexpr bool is_empty(const BoardState& board, uint32_t idx)
{
	return is_empty(board.stones[idx]);
}
inline constexpr void increment_empty_neighbors(BoardState& board, uint32_t idx)
{
	board.neighbor_count[idx] += 1;
}
inline constexpr void decrement_empty_neighbors(BoardState& board, uint32_t idx)
{
	board.neighbor_count[idx] -= 1;
}
inline constexpr void
increment_neighbors(BoardState& board, uint32_t player_idx, uint32_t idx)
{
	board.neighbor_count[idx] += (1UL << (3UL * (player_idx + 1)));
}
inline constexpr void
decrement_neighbors(BoardState& board, uint32_t player_idx, uint32_t idx)
{
	board.neighbor_count[idx] -= (1UL << (3UL * (player_idx + 1)));
}
inline constexpr void
increment_neighbors(BoardState& board, Stone neighbor, uint32_t idx)
{
	increment_neighbors(board, get_player_idx(neighbor), idx);
}
inline constexpr void
decrement_neighbors(BoardState& board, Stone neighbor, uint32_t idx)
{
	decrement_neighbors(board, get_player_idx(neighbor), idx);
}
inline constexpr uint16_t
get_white_neighbors_count(const BoardState& board, uint32_t idx)
{
	return (board.neighbor_count[idx] & uint16_t{0b000111000}) >> 3;
}
inline constexpr uint16_t
get_black_neighbors_count(const BoardState& board, uint32_t idx)
{
	return (board.neighbor_count[idx] & uint16_t{0b111000000}) >> 6;
}
inline constexpr uint16_t
get_empty_neighbors_count(const BoardState& board, uint32_t idx)
{
	return (board.neighbor_count[idx] & uint16_t{0b000000111});
}
inline constexpr void add_empty_stone(BoardState& board, uint32_t idx)
{
	board.empty_stones[board.num_empty_stones++] = idx;
}
inline void remove_empty_stone(BoardState& board, uint32_t idx)
{
	// should probably use a hash table
	for (uint32_t i = 0; i < board.num_empty_stones; i++)
	{
		if (board.empty_stones[i] == idx)
		{
			// not constexpr Q_Q
			std::swap(
			    board.empty_stones[i],
			    board.empty_stones[--board.num_empty_stones]);
			return;
		}
	}
}

struct Action
{
	static constexpr uint16_t PASS = BoardState::INVALID_INDEX;
	static constexpr uint16_t INVALID_ACTION = PASS + 1;

	uint16_t pos;
	uint8_t player_idx;

	constexpr Action(uint16_t pos_, uint8_t player_idx_)
	    : pos{pos_}, player_idx{player_idx_}
	{
	}
	constexpr Action(uint32_t pos_, uint32_t player_idx_)
	    : Action{static_cast<uint16_t>(pos_), static_cast<uint8_t>(player_idx_)}
	{
	}
	constexpr Action() : Action{0u, 0u}
	{
	}
};

inline constexpr bool is_pass(const Action& action)
{
	return action.pos == Action::PASS;
}
inline constexpr bool is_invalid(const Action& action)
{
	return action.pos >= Action::INVALID_ACTION;
}

// A group is a maximal set of connected stones
struct Group
{
	uint16_t parent : 15;
	uint16_t player_idx : 1;
	uint16_t size;
	uint16_t num_libs;
	uint16_t atari_lib;
	MarginRemapped2DBitset<BoardState::MAX_SIZE> lib_map;

	constexpr Group()
	    : parent{0},
	      player_idx{0}, size{0}, num_libs{0}, atari_lib{0}, lib_map{}
	{
	}
};

// A union find structure
struct GroupTable
{
	// checkout https://senseis.xmp.net/?MaximumNumberOfLiveGroups
	static constexpr uint32_t MAX_GROUP_NUM = 277;

	std::array<Group, MAX_GROUP_NUM> groups;
	mutable MarginRemapped2DArray<int16_t, BoardState::MAX_SIZE> parents;

	uint16_t max_group_idx;
	uint16_t num_in_atari;

	// TODO: decide whether to use linked structure to quickly iterate group
	// stones or leave the current implementation
	// mutable std::array<uint16_t, BoardState::EXTENDED_AREA> next_stone;

	constexpr GroupTable()
	    : groups{}, parents{}, max_group_idx{0}, num_in_atari{0}
	{
	}
};

struct Player
{
	uint32_t num_captures;
	uint32_t num_alive;

	constexpr Player() : num_captures{0}, num_alive{0}
	{
	}
};

struct GameState
{
	BoardState board;
	GroupTable group_table;
	std::array<Player, 2> players;
	std::vector<Action> move_history;
	uint8_t player_turn;

	explicit GameState(uint16_t board_size)
	    : board{board_size}, group_table{}, players{}, move_history{},
	      player_turn{0}
	{
	}
	GameState() : GameState(19)
	{
	}
};

// TODO: Support more rules!
struct Rules
{
	float komi = 6.5;
};

} // namespace go::engine

#endif // _ENGINE_BOARD_H_
