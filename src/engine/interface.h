#ifndef SRC_ENGINE_INTERFACE_H_
#define SRC_ENGINE_INTERFACE_H_

#include "board.h"

namespace go
{
namespace engine
{

// Plays a move, if legal, changing the board state and game state
bool make_move(GameState&, const Action&);

bool is_valid_move(const ClusterTable& table, const BoardState&, const Action&);
bool is_suicide_move(
    const ClusterTable& table, const BoardState&, const Action&);
bool is_terminal_state(const GameState&);
void calculate_score(const BoardState&, Player&, Player&);
uint32_t
territory_points(const BoardState&, unsigned char&, uint32_t, uint32_t, bool*);

inline bool is_ko(const BoardState& board_state, const Action& action)
{
	return action.pos == board_state.ko;
}

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_INTERFACE_H_
