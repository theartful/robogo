#ifndef SRC_ENGINE_INTERFACE_H_
#define SRC_ENGINE_INTERFACE_H_

#include "engine/board.h"

namespace go
{
namespace engine
{

// for debugging
enum class MoveValidity
{
	INVALID_NOT_EMPTY = 0,
	INVALID_KO = 1,
	INVALID_SUICIDE = 2,
	INVALID_ACTION_VALUE = 3,
	VALID = 4
};

// Plays a move, if legal, changing the board state and game state
bool make_move(GameState&, const Action&);

MoveValidity
get_move_validity(const ClusterTable& table, const BoardState&, const Action&);
bool is_valid_move(const ClusterTable& table, const BoardState&, const Action&);

bool is_suicide_move(
    const ClusterTable& table, const BoardState&, const Action&);
bool is_terminal_state(const GameState&);
void calculate_score(const BoardState&, Player&, Player&);

inline bool is_ko(const BoardState& board_state, const Action& action)
{
	return action.pos == board_state.ko;
}

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_INTERFACE_H_
