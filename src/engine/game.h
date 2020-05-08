#ifndef _ENGINE_GAME_H_
#define _ENGINE_GAME_H_

#include "board.h"
#include "group.h"

namespace go::engine
{

struct PlayerState
{
	uint32_t num_captures;
	uint32_t num_alive;

	constexpr PlayerState() : num_captures{0}, num_alive{0}
	{
	}
};

struct GameState
{
	BoardState board;
	GroupTable group_table;
	std::array<PlayerState, 2> players;
	std::vector<Action> move_history;
	uint8_t player_turn;

	explicit GameState(uint32_t board_size) :
		board{board_size}, group_table{}, players{}, move_history{},
		player_turn{0}
	{
	}

	GameState() : GameState(19)
	{
	}
};

inline constexpr Stone& get_stone(GameState& game, uint32_t idx)
{
	return get_stone(game.board, idx);
}
inline constexpr const Stone& get_stone(const GameState& game, uint32_t idx)
{
	return get_stone(game.board, idx);
}
inline constexpr const Stone&
get_stone(const GameState& game, uint32_t row, uint32_t col)
{
	return get_stone(game.board, BoardState::index(row, col));
}
inline uint32_t num_captures(GameState& game, uint8_t player_idx)
{
	return game.players[player_idx].num_captures;
}
inline uint32_t num_alive(GameState& game, uint8_t player_idx)
{
	return game.players[player_idx].num_alive;
}

// TODO: Support more rules!
struct Rules
{
	float komi = 6.5;
};

} // namespace go::engine

#endif // _ENGINE_GAME_H_
