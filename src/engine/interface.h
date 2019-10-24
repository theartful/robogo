#ifndef SRC_ENGINE_INTERFACE_H_
#define SRC_ENGINE_INTERFACE_H_

#include "board.h"
#include "config.h"

namespace go
{
namespace engine
{

// Plays a move, if legal, changing the board state and game state
bool make_move(GameState&, const Action&);
// Plays a move, if legal, changing only the board state
bool make_move(BoardState&, const Action&);
bool is_valid_move(const BoardState&, const Action&);
bool is_game_finished(const GameState&);

void calculate_score(const BoardState&, Player&, Player&);
uint32_t
territory_points(const BoardState, unsigned char&, uint32_t, uint32_t, bool*);

void update_suicide_cells(BoardState&);
void update_dead_cells(BoardState&);

bool simulate_suicide(BoardState&, uint32_t, uint32_t, Cell);

void mark_dead(BoardState&, uint32_t, uint32_t);
bool is_empty_cell(Cell);
bool is_suicidal_cell(Cell, uint32_t);
bool is_dead_cell(Cell, uint32_t);
bool is_dead_cell(Cell);

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_INTERFACE_H_
