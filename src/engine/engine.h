#ifndef _ENGINE_ENGINE_H_
#define _ENGINE_ENGINE_H_

#include "game.h"

namespace go::engine
{
// for debugging
enum class MoveLegality
{
	IllegalNotEmpty,
	IllegalKo,
	IllegalSuicide,
	InvalidActionValue,
	Legal
};

// Plays a move, if legal, changing the board state and game state
bool make_move(GameState&, const Action&);
void force_move(GameState&, const Action&);

MoveLegality get_move_legality(const GameState&, const Action&);
bool is_legal_move(const GameState&, const Action&);
bool is_simple_ko(const GameState&, const Action&);
bool is_suicide_move(const GameState&, const Action&);
bool is_terminal_state(const GameState&);

} // namespace go::engine

#endif // _ENGINE_ENGINE_H_
