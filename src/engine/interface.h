#ifndef SRC_ENGINE_INTERFACE_H_
#define SRC_ENGINE_INTERFACE_H_

#include "board.h"
#include "config.h"

namespace go
{
namespace engine
{

// Plays a move, if legal, changing the board state and game state
bool make_move(ClusterTable&, GameState&, const Action&);
// Plays a move, if legal, changing only the board state
bool make_move(ClusterTable&, BoardState&, const Action&);

bool is_valid_move(ClusterTable& table, const BoardState&, const Action&);
bool is_suicide_move(ClusterTable& table, const BoardState&, const Action&);

void kill_cluster(Cluster&, ClusterTable&, BoardState&);

bool is_game_finished(const GameState&);

void calculate_score(const BoardState&, Player&, Player&);
uint32_t
territory_points(const BoardState&, unsigned char&, uint32_t, uint32_t, bool*);

bool is_empty_cell(Cell);
bool is_empty_cell(const BoardState&, uint32_t);

} // namespace engine
} // namespace go

#endif // SRC_ENGINE_INTERFACE_H_
